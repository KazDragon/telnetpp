#include <telnetpp/options/mccp/client.hpp>
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
        [](auto by){return by ^ 0x20;});
    continuation(converted_data);
}

class fake_decompressor : public telnetpp::options::mccp::codec
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

class an_mccp_client : public testing::Test
{
protected:
    fake_decompressor decompressor_;
    telnetpp::options::mccp::client client_{decompressor_}; 
};

class an_active_mccp_client : public an_mccp_client
{
protected:
    an_active_mccp_client()
    {
        client_.negotiate(telnetpp::will, [](auto &&){});
        assert(client_.active());
    }
    
    void receive_data(telnetpp::bytes data)
    {
        decompressor_(
            data,
            [this](telnetpp::bytes decompressed_data, bool compression_end)
            {
                received_data_.insert(
                    received_data_.end(),
                    decompressed_data.begin(),
                    decompressed_data.end());
                    
                compression_ended_ = compression_end;
            });
    }
    
    std::vector<telnetpp::byte> received_data_;
    bool compression_ended_ = false;
};

}

TEST_F(an_mccp_client, reports_mccp_option_code)
{
    ASSERT_EQ(86, client_.option_code());
}

TEST_F(an_active_mccp_client, when_not_engaged_receives_data_in_plain_text)
{
    auto const test_data = "test_data"_tb;
    
    receive_data(test_data);
 
    std::vector<telnetpp::byte> const expected_data{test_data.begin(), test_data.end()};
    ASSERT_EQ(expected_data, received_data_);
}

TEST_F(an_active_mccp_client, receives_compressed_data_after_remote_engages_compression)
{
    client_.subnegotiate({}, [](auto &&){});

    auto const test_data = "test_data"_tb;
    compress_decompress(
        test_data,
        [&](telnetpp::bytes data)
        {
            receive_data(data);
        });
        
    std::vector<telnetpp::byte> const expected_data{test_data.begin(), test_data.end()};
    ASSERT_EQ(expected_data, received_data_);
}

TEST_F(an_active_mccp_client, with_engaged_compression_reports_compression_end_and_no_longer_decompresses)
{
    client_.subnegotiate({}, [](auto &&){});

    auto const test_data = "test_data"_tb;

    decompressor_.end_compression_next_chunk_ = true;
    
    compress_decompress(
        test_data,
        [&](telnetpp::bytes data)
        {
            receive_data(data);
        });
        
    std::vector<telnetpp::byte> const expected_data0{test_data.begin(), test_data.end()};
    ASSERT_EQ(expected_data0, received_data_);
    
    received_data_.clear();
    
    receive_data(test_data);
    std::vector<telnetpp::byte> const expected_data1{test_data.begin(), test_data.end()};
    ASSERT_EQ(expected_data1, received_data_);
}

TEST_F(an_active_mccp_client, deactivated_locally_receives_decompressed_data_until_negotiation_is_complete)
{
    client_.subnegotiate({}, [](auto &&){});
    client_.deactivate([](auto &&){});

    auto const test_data = "test_data"_tb;
    compress_decompress(
        test_data,
        [&](telnetpp::bytes data)
        {
            receive_data(data);
        });

    std::vector<telnetpp::byte> const expected_data{test_data.begin(), test_data.end()};
    ASSERT_EQ(expected_data, received_data_);
    
    received_data_.clear();
    client_.negotiate(telnetpp::wont, [&](auto &&){});
    
    receive_data(test_data);
    
    ASSERT_EQ(expected_data, received_data_);
}

TEST_F(an_active_mccp_client, deactivated_remotely_stops_decompressing_immediately)
{
    client_.subnegotiate({}, [](auto &&){});
    client_.negotiate(telnetpp::wont, [](auto &&){});
        
    auto const test_data = "test_data"_tb;
    
    receive_data(test_data);
 
    std::vector<telnetpp::byte> const expected_data{test_data.begin(), test_data.end()};
    ASSERT_EQ(expected_data, received_data_);
}
