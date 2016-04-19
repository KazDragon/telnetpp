#include <gtest/gtest.h>
#include <telnetpp/options/mccp/codec.hpp>
#include <telnetpp/options/mccp/mccp.hpp>
#include "expect_elements.hpp"
#include <zlib.h>

TEST(mccp_codec_test, sending_empty_tokens_returns_empty_tokens)
{
    telnetpp::options::mccp::codec codec;
    auto tokens = std::vector<telnetpp::stream_token>{};
    auto expected = std::vector<telnetpp::stream_token>{};

    expect_tokens(expected, codec.send(tokens));
}

TEST(mccp_codec_test, uncompressed_sending_data_returns_uncompressed_data)
{
    telnetpp::options::mccp::codec codec;

    auto tokens = std::vector<telnetpp::stream_token>{
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const*>("test") },
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const*>("data") }
    };

    auto expected = std::vector<telnetpp::stream_token>{
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const*>("test") },
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const*>("data") }
    };

    expect_tokens(expected, codec.send(tokens));
}

TEST(mccp_codec_test, uncompressed_any_passes_through_any)
{
    telnetpp::options::mccp::codec codec;
    struct pass_through {};

    auto tokens = std::vector<telnetpp::stream_token> {
        boost::any(pass_through{})
    };

    auto result = codec.send(tokens);

    ASSERT_EQ(size_t{1}, result.size());
    auto any = boost::get<boost::any>(result[0]);
    auto passed_through_token = boost::any_cast<pass_through>(any);
}

TEST(mccp_codec_test, uncompressed_send_and_block_stops_data)
{
    telnetpp::options::mccp::codec codec;

    auto tokens = std::vector<telnetpp::stream_token>{
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const*>("before") },
        boost::any(telnetpp::options::mccp::block_token{}),
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const*>("after") }
    };

    auto expected = std::vector<telnetpp::stream_token>{
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const*>("before") }
    };

    expect_tokens(expected, codec.send(tokens));
}

TEST(mccp_codec_test, uncompressed_blocked_resume_uncompressed_returns_blocked_data_uncompressed)
{
    telnetpp::options::mccp::codec codec;

    auto tokens = std::vector<telnetpp::stream_token>{
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const*>("before") },
        boost::any(telnetpp::options::mccp::block_token{}),
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const*>("after") }
    };

    codec.send(tokens);

    tokens = {
        boost::any(telnetpp::options::mccp::resume_uncompressed_token{})
    };

    auto expected = std::vector<telnetpp::stream_token>{
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const*>("after") }
    };

    expect_tokens(expected, codec.send(tokens));
}

TEST(mccp_codec_test, compressed_blocked_resume_compressed_returns_blocked_data_compressed)
{
    telnetpp::options::mccp::codec codec;

    auto tokens = std::vector<telnetpp::stream_token>{
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const*>("before") },
        boost::any(telnetpp::options::mccp::block_token{}),
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const*>("after") }
    };

    codec.send(tokens);

    auto expected =
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const*>("after") };

    tokens = {
        boost::any(telnetpp::options::mccp::resume_compressed_token{})
    };

    // Rather than duplicating the work and seeing what the compressed output
    // should look like, we instead uncompress the result of the operation.
    // This should be identical to the sent data.
    auto response = codec.send(tokens);
    ASSERT_EQ(size_t{1}, response.size());
    auto data = boost::get<telnetpp::u8stream>(response[0]);

    z_stream stream = {};
    int result = inflateInit(&stream);
    assert(result == Z_OK);

    telnetpp::u8 bytes[1024] = {};
    std::copy(data.begin(), data.end(), bytes);
    telnetpp::u8 output[1024] = {};

    stream.avail_in = data.size();
    stream.next_in = bytes;
    stream.avail_out = sizeof(output);
    stream.next_out = output;

    result = inflate(&stream, Z_SYNC_FLUSH);

    ASSERT_EQ(Z_OK, result);

    auto output_end = output + (sizeof(output) - stream.avail_out);
    telnetpp::u8stream actual(output, output_end);
    ASSERT_EQ(expected, actual);

    inflateEnd(&stream);
}

TEST(mccp_codec_test, compressed_sending_data_returns_compressed_data)
{
    telnetpp::options::mccp::codec codec;

    auto tokens = std::vector<telnetpp::stream_token> {
        boost::any(telnetpp::options::mccp::block_token{}),
        boost::any(telnetpp::options::mccp::resume_compressed_token{}),
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const*>("data") }
    };

    auto expected =
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const*>("data") };

    auto response = codec.send(tokens);

    ASSERT_EQ(size_t{1}, response.size());
    auto data = boost::get<telnetpp::u8stream>(response[0]);

    z_stream stream = {};
    int result = inflateInit(&stream);
    assert(result == Z_OK);

    telnetpp::u8 bytes[1024] = {};
    std::copy(data.begin(), data.end(), bytes);
    telnetpp::u8 output[1024] = {};

    stream.avail_in = data.size();
    stream.next_in = bytes;
    stream.avail_out = sizeof(output);
    stream.next_out = output;

    result = inflate(&stream, Z_SYNC_FLUSH);

    ASSERT_EQ(Z_OK, result);

    auto output_end = output + (sizeof(output) - stream.avail_out);
    telnetpp::u8stream actual(output, output_end);
    ASSERT_EQ(expected, actual);

    inflateEnd(&stream);

}

TEST(mccp_codec_test, compressed_block_blocks_data)
{
    telnetpp::options::mccp::codec codec;

    auto tokens = std::vector<telnetpp::stream_token> {
        boost::any(telnetpp::options::mccp::resume_compressed_token{}),
        boost::any(telnetpp::options::mccp::block_token{}),
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const*>("data") }
    };

    auto expected = std::vector<telnetpp::stream_token> {};

    expect_tokens(expected, codec.send(tokens));
}

TEST(mccp_codec_test, uncompressed_receive_empty_data_returns_empty_data)
{
    telnetpp::options::mccp::codec codec;

    auto tokens = std::vector<telnetpp::stream_token> {};

    auto result = codec.receive(tokens);

    ASSERT_FALSE(result.is_initialized());
}

TEST(mccp_codec_test, uncompressed_receive_data_returns_data)
{
    telnetpp::options::mccp::codec codec;

    auto tokens = std::vector<telnetpp::stream_token> {
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const *>("data") }
    };

    auto expected = std::vector<telnetpp::stream_token> {
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const *>("data") }
    };

    auto result = codec.receive(tokens);

    ASSERT_TRUE(result.is_initialized());

    expect_tokens(expected, *result);
}