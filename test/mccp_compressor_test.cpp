#include <gtest/gtest.h>
#include <telnetpp/options/mccp/compressor.hpp>
#include <telnetpp/options/mccp/mccp.hpp>
#include "expect_elements.hpp"
#include <random>
#include <zlib.h>

TEST(mccp_compressor_test, sending_empty_tokens_returns_empty_tokens)
{
    telnetpp::options::mccp::compressor compressor;
    auto tokens = std::vector<telnetpp::stream_token>{};
    auto expected = std::vector<telnetpp::stream_token>{};

    expect_tokens(expected, compressor.send(tokens));
}

TEST(mccp_compressor_test, uncompressed_sending_data_returns_uncompressed_data)
{
    telnetpp::options::mccp::compressor compressor;

    auto tokens = std::vector<telnetpp::stream_token>{
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const*>("test") },
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const*>("data") }
    };

    auto expected = std::vector<telnetpp::stream_token>{
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const*>("test") },
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const*>("data") }
    };

    expect_tokens(expected, compressor.send(tokens));
}

TEST(mccp_compressor_test, uncompressed_any_passes_through_any)
{
    telnetpp::options::mccp::compressor compressor;
    struct pass_through {};

    auto tokens = std::vector<telnetpp::stream_token> {
        boost::any(pass_through{})
    };

    auto result = compressor.send(tokens);

    ASSERT_EQ(size_t{1}, result.size());
    auto any = boost::get<boost::any>(result[0]);
    auto passed_through_token = boost::any_cast<pass_through>(any);
}

TEST(mccp_compressor_test, uncompressed_end_compression_returns_no_data)
{
    telnetpp::options::mccp::compressor compressor;

    auto tokens = std::vector<telnetpp::stream_token>{
        boost::any(telnetpp::options::mccp::end_compression{}),
    };

    auto result = compressor.send(tokens);

    auto expected = std::vector<telnetpp::stream_token>{
    };

    expect_tokens(expected, result);
}

TEST(mccp_compressor_test, uncompressed_end_compression_returns_data_uncompressed)
{
    telnetpp::options::mccp::compressor compressor;

    auto tokens = std::vector<telnetpp::stream_token>{
        boost::any(telnetpp::options::mccp::end_compression{}),
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const*>("data") }
    };

    auto result = compressor.send(tokens);

    auto expected = std::vector<telnetpp::stream_token>{
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const*>("data") }
    };

    expect_tokens(expected, result);
}

TEST(mccp_compressor_test, uncompressed_begin_compression_returns_no_data)
{
    telnetpp::options::mccp::compressor compressor;

    auto tokens = std::vector<telnetpp::stream_token>{
        boost::any(telnetpp::options::mccp::begin_compression{}),
    };

    auto result = compressor.send(tokens);

    auto expected = std::vector<telnetpp::stream_token>{
    };

    expect_tokens(expected, result);
}

TEST(mccp_compressor_test, uncompressed_begin_compression_returns_data_compressed)
{
    telnetpp::options::mccp::compressor compressor;

    auto tokens = std::vector<telnetpp::stream_token>{
        boost::any(telnetpp::options::mccp::begin_compression{}),
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const*>("data") }
    };

    auto expected =
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const*>("data") };
        
    auto result = compressor.send(tokens);

    // Rather than duplicating the work and seeing what the compressed output
    // should look like, we instead uncompress the result of the operation.
    // This should be identical to the sent data.
    ASSERT_EQ(size_t{1}, result.size());
    auto data = boost::get<telnetpp::u8stream>(result[0]);

    z_stream stream = {};
    int response = inflateInit(&stream);
    assert(response == Z_OK);

    telnetpp::u8 bytes[1024] = {};
    std::copy(data.begin(), data.end(), bytes);
    telnetpp::u8 output[1024] = {};

    stream.avail_in = data.size();
    stream.next_in = bytes;
    stream.avail_out = sizeof(output);
    stream.next_out = output;

    response = inflate(&stream, Z_SYNC_FLUSH);
    ASSERT_EQ(Z_OK, response);

    auto output_end = output + (sizeof(output) - stream.avail_out);
    telnetpp::u8stream actual(output, output_end);
    ASSERT_EQ(expected, actual);

    inflateEnd(&stream);
}

TEST(mccp_compressor_test, restarting_compressed_stream_sends_new_compressed_data)
{
    telnetpp::options::mccp::compressor compressor;

    auto tokens = std::vector<telnetpp::stream_token> {
        boost::any(telnetpp::options::mccp::begin_compression{}),
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const *>("data0") },
        boost::any(telnetpp::options::mccp::begin_compression{})
    };

    auto response = compressor.send(tokens);
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
    assert(result ==  Z_OK);
    inflateEnd(&stream);

    tokens = std::vector<telnetpp::stream_token> {
        boost::any(telnetpp::options::mccp::begin_compression{}),
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const *>("data1") },
    };

    response = compressor.send(tokens);
    data = boost::get<telnetpp::u8stream>(response[0]);

    stream = {};
    result = inflateInit(&stream);
    assert(result == Z_OK);

    std::copy(data.begin(), data.end(), bytes);

    stream.avail_in  = data.size();
    stream.next_in   = bytes;
    stream.avail_out = sizeof(output);
    stream.next_out  = output;

    result = inflate(&stream, Z_SYNC_FLUSH);
    ASSERT_EQ(Z_OK, result);
    inflateEnd(&stream);

    auto expected =
        telnetpp::u8stream{ reinterpret_cast<telnetpp::u8 const*>("data1") };

    auto output_end = output + (sizeof(output) - stream.avail_out);
    telnetpp::u8stream actual(output, output_end);
    ASSERT_EQ(expected, actual);
}

TEST(mccp_compressor_test, compressed_large_stream_sent_correctly)
{
    // In the compression compressor, when streams are compressed, they are
    // returned in small chunks that must be reassembled.  The default
    // chunk size is 1023 bytes.  Therefore, to test this, we attempt
    // to compress a batch of random (and thus not easily compressible)
    // data that should compress to much more than that.
    static size_t const stream_size = 64000;

    telnetpp::u8stream stream(stream_size, '\0');
    assert(stream.size() ==  stream_size);

    std::generate(stream.begin(), stream.end(),
        []{
            static std::random_device rdev;
            static std::mt19937 rng{rdev()};
            static std::uniform_int_distribution<> distribution(0, 255);
            return distribution(rng);
        });

    telnetpp::options::mccp::compressor compressor;

    auto response = compressor.send({
        boost::any{telnetpp::options::mccp::begin_compression{}},
        stream
    });

    ASSERT_EQ(size_t{1}, response.size());
    auto compressed_stream = boost::get<telnetpp::u8stream>(response[0]);

    // The compressed stream must be greater than this, or otherwise we haven't
    // actually tested the use case.
    // Note: When measured, this actually "compressed" to 64027 bytes.  So
    // it looks like random numbers is a *really* good test.
    assert(compressed_stream.size() > 1023u);

    // Now inflate the stream to ensure that it was completely compressed.
    z_stream inflate_stream = {};
    int result = inflateInit(&inflate_stream);
    assert(result == Z_OK);

    telnetpp::u8stream output(stream_size, '\0');

    inflate_stream.avail_in  = compressed_stream.size();
    inflate_stream.next_in   = &compressed_stream[0];
    inflate_stream.avail_out = stream_size;
    inflate_stream.next_out  = &output[0];

    result = inflate(&inflate_stream, Z_SYNC_FLUSH);
    assert(result == Z_OK);
    inflateEnd(&inflate_stream);

    // Because the initial stream and the expected decompressed stream are the
    // same size, there should be no space left in the output stream.
    // Otherwise, the stream was clipped in some way.
    ASSERT_EQ(0, inflate_stream.avail_out);
    ASSERT_EQ(stream, output);
}

TEST(mccp_compressor_test, compressed_any_passes_through_any)
{
    telnetpp::options::mccp::compressor compressor;
    struct pass_through {};

    auto tokens = std::vector<telnetpp::stream_token> {
        boost::any(telnetpp::options::mccp::begin_compression{}),
        boost::any(pass_through{})
    };

    auto result = compressor.send(tokens);

    ASSERT_EQ(size_t{1}, result.size());
    auto any = boost::get<boost::any>(result[0]);
    auto passed_through_token = boost::any_cast<pass_through>(any);
}

TEST(mccp_compressor_test, compressed_end_compression_returns_compression_end)
{
    telnetpp::options::mccp::compressor compressor;
    
    auto tokens = std::vector<telnetpp::stream_token> {
        boost::any(telnetpp::options::mccp::begin_compression{}),
        boost::any(telnetpp::options::mccp::end_compression{})
    };
    
    auto result = compressor.send(tokens);
    
    ASSERT_EQ(size_t{1}, result.size());
    auto compressed_stream = boost::get<telnetpp::u8stream>(result[0]);
    
    z_stream inflate_stream = {};
    int response = inflateInit(&inflate_stream);
    assert(response == Z_OK);

    telnetpp::u8stream output(1024, '\0');

    inflate_stream.avail_in  = compressed_stream.size();
    inflate_stream.next_in   = &compressed_stream[0];
    inflate_stream.avail_out = sizeof(output);
    inflate_stream.next_out  = &output[0];

    response = inflate(&inflate_stream, Z_SYNC_FLUSH);
    ASSERT_EQ(Z_STREAM_END, response);
    ASSERT_EQ(sizeof(output), inflate_stream.avail_out);
    inflateEnd(&inflate_stream);
}
