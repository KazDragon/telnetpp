#include "expect_elements.hpp"
#include <telnetpp/options/mccp/detail/zlib_compressor.hpp>
//#include <telnetpp/options/mccp/codec.hpp>
//#include <telnetpp/options/mccp/mccp.hpp>
#include <random>
#include <zlib.h>
#include <gtest/gtest.h>

TEST(mccp_zlib_compressor_test, zlib_compressor_is_compressor)
{
    // The MCCP codec will rely on the compressor interface, so 
    // zlib_compressor must implement that.
    telnetpp::options::mccp::detail::zlib_compressor zlib_compressor;
    telnetpp::options::mccp::detail::compressor &compressor = zlib_compressor;
}

TEST(mccp_zlib_compressor_test, compressing_empty_stream_returns_empty_stream)
{
    // If the compressor receives an empty stream of data to compress, then
    // it is not worth sending any data at all, so the compressor must return
    // an empty stream of data.
    telnetpp::options::mccp::detail::zlib_compressor zlib_compressor;
    telnetpp::options::mccp::detail::compressor &compressor = zlib_compressor;
    
    auto const expected = telnetpp::u8stream{};
    
    ASSERT_EQ(expected, compressor.compress(telnetpp::u8stream{}));
}

TEST(mccp_zlib_compressor_test, compressing_data_returns_data_compressed)
{
    // If the compressor receives a stream of data, then it must compress that
    // stream and return the compressed content. To test this, we then 
    // uncompress the stream and assert that it is the same as the original
    // content.
    telnetpp::options::mccp::detail::zlib_compressor zlib_compressor;
    telnetpp::options::mccp::detail::compressor &compressor = zlib_compressor;

    auto const expected = telnetpp::u8stream{
        'd', 'a', 't', 'a', 'd', 'a', 't', 'a', 'd', 'a', 't', 'a',
        'd', 'a', 't', 'a', 'd', 'a', 't', 'a', 'd', 'a', 't', 'a'
    };
    
    auto const result = compressor.compress(expected);
    
    z_stream stream = {};
    int response = inflateInit(&stream);
    assert(response == Z_OK);
    
    telnetpp::u8 output_buffer[1024] = {};

    stream.avail_in = result.size();
    stream.next_in = const_cast<telnetpp::u8 *>(&result[0]);
    stream.avail_out = sizeof(output_buffer);
    stream.next_out = output_buffer;

    response = inflate(&stream, Z_SYNC_FLUSH);
    ASSERT_EQ(Z_OK, response);
    inflateEnd(&stream);

    auto output_end = 
        output_buffer + (sizeof(output_buffer) - stream.avail_out);
    telnetpp::u8stream actual(output_buffer, output_end);
    
    ASSERT_EQ(expected, actual);
}

TEST(mccp_zlib_compressor_test, compressing_more_data_returns_more_data_compressed)
{
    // A compressed stream relies on previous data in order to build a
    // dictionary of compressable artifacts.  This means that a second batch
    // of data may be received and return a different response to the first
    // batch.  To test this, we send the same data twice, and ensure that the
    // second batch of data is also received correctly.
    telnetpp::options::mccp::detail::zlib_compressor zlib_compressor;
    telnetpp::options::mccp::detail::compressor &compressor = zlib_compressor;

    auto const expected = telnetpp::u8stream{
        'd', 'a', 't', 'a', 'd', 'a', 't', 'a', 'd', 'a', 't', 'a',
        'd', 'a', 't', 'a', 'd', 'a', 't', 'a', 'd', 'a', 't', 'a'
    };
    
    auto result = compressor.compress(expected);
    
    z_stream stream = {};
    int response = inflateInit(&stream);
    assert(response == Z_OK);
    
    telnetpp::u8 output_buffer[1024] = {};

    stream.avail_in  = result.size();
    stream.next_in   = &result[0];
    stream.avail_out = sizeof(output_buffer);
    stream.next_out  = output_buffer;

    response = inflate(&stream, Z_SYNC_FLUSH);
    assert(response == Z_OK);
    
    result = compressor.compress(expected);

    stream.avail_in  = result.size();
    stream.next_in   = &result[0];
    stream.avail_out = sizeof(output_buffer);
    stream.next_out  = output_buffer;

    response = inflate(&stream, Z_SYNC_FLUSH);
    ASSERT_EQ(Z_OK, response);
    
    inflateEnd(&stream);

    auto output_end = 
        output_buffer + (sizeof(output_buffer) - stream.avail_out);
    telnetpp::u8stream actual(output_buffer, output_end);
    
    ASSERT_EQ(expected, actual);
}

TEST(mccp_compressor_test, end_compression_returns_compression_end_sequence)
{
    // It is possible to end the compression stream.  In this case, a sequence
    // is returned that denotes the end of the compression stream.
    telnetpp::options::mccp::detail::zlib_compressor zlib_compressor;
    telnetpp::options::mccp::detail::compressor &compressor = zlib_compressor;
    
    auto result = compressor.end_compression();

    z_stream stream = {};
    int response = inflateInit(&stream);
    assert(response == Z_OK);
    
    telnetpp::u8 output_buffer[1024] = {};

    stream.avail_in  = result.size();
    stream.next_in   = &result[0];
    stream.avail_out = sizeof(output_buffer);
    stream.next_out  = output_buffer;

    response = inflate(&stream, Z_SYNC_FLUSH);
    ASSERT_EQ(Z_STREAM_END, response);
}

TEST(mccp_compressor_test, ending_compression_then_compressing_data_restarts_compression_stream)
{
    // After an end_compression, the next time compress() is called, the 
    // stream must begin as if new.
    telnetpp::options::mccp::detail::zlib_compressor zlib_compressor;
    telnetpp::options::mccp::detail::compressor &compressor = zlib_compressor;
    compressor.end_compression();
    
    auto const expected = telnetpp::u8stream{
        'd', 'a', 't', 'a', 'd', 'a', 't', 'a', 'd', 'a', 't', 'a',
        'd', 'a', 't', 'a', 'd', 'a', 't', 'a', 'd', 'a', 't', 'a'
    };
    
    auto const result = compressor.compress(expected);
    
    z_stream stream = {};
    int response = inflateInit(&stream);
    assert(response == Z_OK);
    
    telnetpp::u8 output_buffer[1024] = {};

    stream.avail_in = result.size();
    stream.next_in = const_cast<telnetpp::u8 *>(&result[0]);
    stream.avail_out = sizeof(output_buffer);
    stream.next_out = output_buffer;

    response = inflate(&stream, Z_SYNC_FLUSH);
    ASSERT_EQ(Z_OK, response);
    inflateEnd(&stream);

    auto output_end = 
        output_buffer + (sizeof(output_buffer) - stream.avail_out);
    telnetpp::u8stream actual(output_buffer, output_end);
    
    ASSERT_EQ(expected, actual);
}

TEST(mccp_compressor_test, can_compress_large_data)
{
    // When streams are compressed, they are returned in small chunks that 
    // must be reassembled.  The default chunk size is 1023 bytes.  Therefore,
    // to test this reassembly, we attempt to compress a batch of random (and
    // thus not easily compressible) data that should compress to much more
    // than that.
    static size_t const stream_size = 64000;

    telnetpp::u8stream stream(stream_size, '\0');
    assert(stream.size() == stream_size);

    std::generate(stream.begin(), stream.end(),
        []{
            static std::random_device rdev;
            static std::mt19937 rng{rdev()};
            static std::uniform_int_distribution<> distribution(0, 255);
            return distribution(rng);
        });

    telnetpp::options::mccp::detail::zlib_compressor zlib_compressor;
    telnetpp::options::mccp::detail::compressor &compressor = zlib_compressor;
    auto compressed_stream = compressor.compress(stream);
    
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
