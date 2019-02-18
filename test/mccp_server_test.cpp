#include <telnetpp/options/mccp/server.hpp>
#include <telnetpp/options/mccp/codec.hpp>
#include <gtest/gtest.h>

using namespace telnetpp::literals;

namespace {
    
// Reversible "compression" function.
template <typename Continuation>
void compress_decompress(
    telnetpp::bytes data,
    Continuation &&continuation)
{
    std::vector<telnetpp::byte> converted_data(data.size());
    std::transform(
        data.begin(),
        data.end(),
        converted_data.begin(),
        [](auto by){return by ^ 0x55;});
    continuation(converted_data);
}

class fake_compressor : public telnetpp::options::mccp::codec
{
public:
    bool end_compression_next_chunk_ = false;

private:
    telnetpp::bytes transform_chunk(
      telnetpp::bytes data,
      std::function<void (telnetpp::bytes, bool)> const &continuation) override
    {
        compress_decompress(
            data,
            [&](auto const &data)
            {
                continuation(data, end_compression_next_chunk_);
            });

        return telnetpp::bytes{};
    }
};

class an_mccp_server : public testing::Test
{
protected:
    void send_data(telnetpp::bytes data)
    {
        compressor_(
            data,
            [this](telnetpp::bytes compressed_data, bool compression_end)
            {
                sent_data_.insert(
                    sent_data_.end(),
                    compressed_data.begin(),
                    compressed_data.end());
                    
                compression_ended_ = compression_end;
            });
    }

    fake_compressor compressor_;
    telnetpp::options::mccp::server server_{compressor_};
    
    std::vector<telnetpp::byte> sent_data_;
    bool compression_ended_ = false;
};

}

TEST_F(an_mccp_server, reports_mccp_option_code)
{
    ASSERT_EQ(86, server_.option_code());
}

TEST_F(an_mccp_server, sends_nothing_when_beginning_compression)
{
    auto received_data = std::vector<telnetpp::element>{};
    server_.begin_compression(
        [&](auto const &elem)
        {
            received_data.push_back(elem);
        });

    auto const expected_data = std::vector<telnetpp::element>{};
    ASSERT_EQ(expected_data, received_data);
}

TEST_F(an_mccp_server, sends_nothing_when_being_activated_locally)
{
    // In particular, we mean when the confirmation of our locally initiated
    // negotiation returns.
    auto received_data = std::vector<telnetpp::element>{};
    server_.activate([](auto &&){});
    server_.negotiate(
        telnetpp::do_, 
        [&](auto const &elem)
        {
            received_data.push_back(elem);
        });
    assert(server_.active());
        
    auto const expected_data = std::vector<telnetpp::element>{};
    ASSERT_EQ(expected_data, received_data);
}

TEST_F(an_mccp_server, sends_nothing_when_being_activated_remotely)
{
    // In particular, we mean apart from our confirmation of the
    // negotiation.
    auto received_data = std::vector<telnetpp::element>{};
    server_.negotiate(
        telnetpp::do_, 
        [&](auto const &elem)
        {
            received_data.push_back(elem);
        });
    assert(server_.active());
    
    auto const expected_data = std::vector<telnetpp::element>{
        telnetpp::element{telnetpp::negotiation{telnetpp::will, 86}}
    };
    
    ASSERT_EQ(expected_data, received_data);
}

namespace {

class an_mccp_server_with_compression_started
  : public an_mccp_server
{
protected:
    an_mccp_server_with_compression_started()
    {
        server_.begin_compression([](auto &&){});
    }
};

}

TEST_F(an_mccp_server_with_compression_started, sends_compression_subnegotiation_when_activated_locally)
{
    auto received_data = std::vector<telnetpp::element>{};
    server_.activate([](auto &&){});
    server_.negotiate(
        telnetpp::do_, 
        [&](auto const &elem)
        {
            received_data.push_back(elem);
        });
    assert(server_.active());
        
    auto const expected_data = std::vector<telnetpp::element>{
        telnetpp::element{telnetpp::subnegotiation{86, {}}}
    };
    ASSERT_EQ(expected_data, received_data);
}

TEST_F(an_mccp_server_with_compression_started, sends_compression_subnegotiation_when_activated_remotely)
{
    auto received_data = std::vector<telnetpp::element>{};
    server_.negotiate(
        telnetpp::do_, 
        [&](auto const &elem)
        {
            received_data.push_back(elem);
        });
    assert(server_.active());
        
    auto const expected_data = std::vector<telnetpp::element>{
        telnetpp::element{telnetpp::negotiation{telnetpp::will, 86}},
        telnetpp::element{telnetpp::subnegotiation{86, {}}}
    };
    ASSERT_EQ(expected_data, received_data);
}

namespace {
    
class an_active_mccp_server_with_compression_started
  : public an_mccp_server_with_compression_started
{
protected:
    an_active_mccp_server_with_compression_started()
    {
        server_.negotiate(
            telnetpp::do_, 
            [&](auto const &elem){});
        assert(server_.active());
    }
};

}

TEST_F(an_active_mccp_server_with_compression_started, sends_compressed_data)
{
    auto const test_data = "test_data"_tb;
    send_data(test_data);
    
    std::vector<telnetpp::byte> const expected_data = [&]()
    {
        std::vector<telnetpp::byte> data;
        
        compress_decompress(
            test_data,
            [&](auto const &bytes)
            {
                data.insert(data.end(), bytes.begin(), bytes.end());
            });
            
        return data;
    }();
    
    ASSERT_EQ(expected_data, sent_data_);
}

TEST_F(an_mccp_server, sends_nothing_when_ending_compression)
{
    auto received_data = std::vector<telnetpp::element>{};
    server_.end_compression(
        [&](auto const &elem)
        {
            received_data.push_back(elem);
        });

    auto const expected_data = std::vector<telnetpp::element>{};
    ASSERT_EQ(expected_data, received_data);
}

namespace {

class an_mccp_server_with_compression_ended
  : public an_mccp_server
{
protected:
    an_mccp_server_with_compression_ended()
    {
        server_.end_compression([](auto &&){});
    }
};

}

/*
TEST_F(an_mccp_server, deactivated_end_compression_then_activate_sends_nothing)
{
    telnetpp::options::mccp::server server;
    server.end_compression();

    auto const expected = std::vector<telnetpp::token> {
    };

    server.activate();
    auto const result = server.negotiate(telnetpp::do_);

    expect_tokens(expected, result);
}

TEST_F(an_mccp_server, deactivated_begin_compression_end_compression_then_activate_sends_nothing)
{
    telnetpp::options::mccp::server server;
    server.begin_compression();
    server.end_compression();

    auto const expected = std::vector<telnetpp::token> {
    };

    server.activate();
    auto const result = server.negotiate(telnetpp::do_);

    expect_tokens(expected, result);
}

TEST_F(an_mccp_server, activated_begin_compression_sends_begin_compression_sequence)
{
    telnetpp::options::mccp::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    assert(server.is_active());

    auto const expected = std::vector<telnetpp::token> {
        telnetpp::element(telnetpp::subnegotiation(
            server.option(), {})),
        boost::any(telnetpp::options::mccp::detail::begin_compression{})
    };

    auto const result = server.begin_compression();

    expect_tokens(expected, result);
}

TEST_F(an_mccp_server, activated_end_compression_sends_end_compression_token)
{
    telnetpp::options::mccp::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    assert(server.is_active());

    auto const expected = std::vector<telnetpp::token> {
        boost::any(telnetpp::options::mccp::detail::end_compression{})
    };

    auto const result = server.end_compression();

    expect_tokens(expected, result);
}

TEST_F(an_mccp_server, activated_compressed_deactivate_ends_compression)
{
    telnetpp::options::mccp::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    server.begin_compression();

    // If we end compression, we expect two tokens to be output.  The first
    // will be the (compressed) WONT MCCP negotiation.  Since the only valid
    // response to this is DONT MCCP by the client, the second token will
    // be an end_compression token, as we can assume that the client will
    // immediately end compression upon reception of the negotiation.
    auto const result = server.deactivate();

    ASSERT_EQ(size_t{2}, result.size());
    auto const &token = boost::get<boost::any>(result[1]);
    boost::any_cast<telnetpp::options::mccp::detail::end_compression>(token);
}

TEST_F(an_mccp_server, activated_compressed_negotiate_deactivation_ends_compression)
{
    telnetpp::options::mccp::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    server.begin_compression();

    // If we end compression, we expect two tokens to be output.  The first
    // will be the (compressed) WONT MCCP negotiation.  Since the only valid
    // response to this is DONT MCCP by the client, the second token will
    // be an end_compression token, as we can assume that the client will
    // immediately end compression upon reception of the negotiation.
    auto const result = server.negotiate(telnetpp::dont);

    ASSERT_EQ(size_t{2}, result.size());
    auto const &token = boost::get<boost::any>(result[1]);
    boost::any_cast<telnetpp::options::mccp::detail::end_compression>(token);
}
*/
