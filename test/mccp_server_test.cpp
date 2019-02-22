#include <telnetpp/options/mccp/server.hpp>
#include <telnetpp/options/mccp/codec.hpp>
#include <telnetpp/detail/lambda_visitor.hpp>
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
        [](auto by){return by ^ 0x20;});
    continuation(converted_data);
}

class fake_compressor : public telnetpp::options::mccp::codec
{
public:
    bool end_compression_next_chunk_ = false;
    bool finished_ = false;

private:
    telnetpp::bytes transform_chunk(
      telnetpp::bytes data,
      continuation const &cont) override
    {
        compress_decompress(
            data,
            [&](auto const &data)
            {
                cont(data, end_compression_next_chunk_);
            });

        return telnetpp::bytes{};
    }

    void do_finish(continuation const &cont) override
    {
        auto const marker = "end"_tb;
        cont(marker, true);
        finished_ = true;
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
    server_.start_compression(
        [&](auto const &elem)
        {
            received_data.push_back(elem);
        });

    auto const expected_data = std::vector<telnetpp::element>{};
    ASSERT_EQ(expected_data, received_data);
}

TEST_F(an_mccp_server, sends_nothing_when_finishing_compression)
{
    auto received_data = std::vector<telnetpp::element>{};
    server_.finish_compression(
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

class an_active_mccp_server : public an_mccp_server
{
protected:
    an_active_mccp_server()
    {
        server_.negotiate(
            telnetpp::do_, 
            [&](auto const &elem){});
        assert(server_.active());
    }
};

}

TEST_F(an_active_mccp_server, sends_empty_subnegotiation_when_beginning_compression)
{
    auto const expected = std::vector<telnetpp::element> {
        telnetpp::subnegotiation{server_.option_code(), {}}
    };
    
    auto received = std::vector<telnetpp::element>{};
    server_.start_compression(
        [&](auto const &element)
        {
            received.push_back(element);
        });
        
    ASSERT_EQ(expected, received);
}

TEST_F(an_active_mccp_server, sends_nothing_when_finishing_compression)
{
    auto const expected_data = std::vector<telnetpp::byte>{};
    
    server_.finish_compression(
        [&](auto const &elements)
        {
            auto const &data = boost::get<telnetpp::bytes>(elements);
            sent_data_.insert(
                sent_data_.end(),
                data.begin(),
                data.end());
        });
        
    ASSERT_EQ(expected_data, sent_data_);
}

TEST_F(an_active_mccp_server, sends_data_uncompressed)
{
    auto const test_data = "test_data"_tb;
    send_data(test_data);

    auto const expected_data = std::vector<telnetpp::byte>{
        test_data.begin(), test_data.end()
    };

    ASSERT_EQ(expected_data, sent_data_);
}

namespace {

class an_mccp_server_activated_with_compression_started 
  : public an_active_mccp_server
{
protected:
    an_mccp_server_activated_with_compression_started()
    {
        server_.start_compression([&](auto const &elem){});
    }
};

}

TEST_F(an_mccp_server_activated_with_compression_started, sends_compressed_data)
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


namespace {

class an_active_mccp_server_with_compression_started 
  : public an_active_mccp_server
{
protected:
    an_active_mccp_server_with_compression_started()
    {
        server_.start_compression([](auto &&){});
    }
};

}

TEST_F(an_active_mccp_server_with_compression_started, sends_data_compressed)
{
    auto const test_data = "test_data"_tb;
    send_data(test_data);

    auto const expected_data = [&]()
    {
        std::vector<telnetpp::byte> compressed_data;
        compress_decompress(
            test_data,
            [&](auto const &bytes)
            {
                compressed_data.insert(
                    compressed_data.end(),
                    bytes.begin(),
                    bytes.end());
            });

        return compressed_data;
    }();

    ASSERT_EQ(expected_data, sent_data_);
}

TEST_F(an_active_mccp_server_with_compression_started, sends_compression_end_compressed_when_compression_is_ended)
{
    server_.finish_compression(
        [this](telnetpp::element const &element)
        {
            send_data(boost::get<telnetpp::bytes>(element));
        });

    auto const end_data = "end"_tb;
    std::vector<telnetpp::byte> const expected_data = [&]()
    {
        std::vector<telnetpp::byte> data;
        
        compress_decompress(
            end_data,
            [&](auto const &bytes)
            {
                data.insert(data.end(), bytes.begin(), bytes.end());
            });
            
        return data;
    }();
    
    ASSERT_EQ(expected_data, sent_data_);
    ASSERT_TRUE(compressor_.finished_);
}

TEST_F(an_active_mccp_server_with_compression_started, sends_compression_end_compressed_and_negotiation_uncompressed)
{
    server_.negotiate(
        telnetpp::dont,
        [this](telnetpp::element const &element)
        {
            telnetpp::detail::visit_lambdas(
                element,
                [this](telnetpp::bytes data)
                {
                    send_data(data);
                },
                [this](telnetpp::negotiation neg)
                {
                    telnetpp::byte const output[] = {
                        telnetpp::iac,
                        neg.request(),
                        neg.option()
                    };
                    
                    send_data(output);
                },
                [](auto &&)
                {
                    FAIL();
                });
        });
        
    std::vector<telnetpp::byte> const expected_data = [&]()
    {
        std::vector<telnetpp::byte> data;
        
        auto const end_data = "end"_tb;
        compress_decompress(
            end_data,
            [&](auto const &bytes)
            {
                data.insert(data.end(), bytes.begin(), bytes.end());
            });
            
        telnetpp::byte const neg_data[] = {
            telnetpp::iac,
            telnetpp::wont,
            server_.option_code()
        };
        
        using std::begin;
        using std::end;
        
        data.insert(data.end(), begin(neg_data), end(neg_data));
        return data;
    }();
    
    ASSERT_EQ(expected_data, sent_data_);
    ASSERT_TRUE(compressor_.finished_);
}

/*
TEST_F(an_mccp_server, activated_compressed_deactivate_ends_compression)
{
    telnetpp::options::mccp::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    server.start_compression();

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
    server.start_compression();

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
