#include <telnetpp/options/mccp/codec.hpp>
#include <telnetpp/options/mccp/mccp.hpp>
#include <gtest/gtest.h>
#include <zlib.h>

TEST(mccp_decompressor_test, uncompressed_receive_byte_returns_byte)
{
    // In uncompressed mode, reception of a byte should only result in a
    // byte stream of one element which is the received byte.
    telnetpp::options::mccp::codec codec;

    auto const &expected = telnetpp::u8stream{ 'T' };

    ASSERT_EQ(expected, codec.receive('T'));
}

TEST(mccp_decompressor_test, compressed_receive_compressed_data_returns_data)
{
    // If the stream is expecting compressed data, then receiving compressed
    // data should be interpreted over time as the original data.
    telnetpp::options::mccp::codec codec;
    codec.send({
        boost::any(telnetpp::options::mccp::begin_decompression{})
    });

    telnetpp::u8stream expected = { 'd', 'a', 't', 'a', 'd', 'a', 't', 'a' };

    z_stream stream = {};
    auto response = deflateInit(&stream, Z_DEFAULT_COMPRESSION);
    assert(response == Z_OK);

    telnetpp::u8 buffer[1023];
    stream.next_in = &expected[0];
    stream.avail_in = expected.size();
    stream.next_out = buffer;
    stream.avail_out = sizeof(buffer);

    response = deflate(&stream, Z_SYNC_FLUSH);
    assert(response == Z_OK);

    auto amount_received = sizeof(buffer) - stream.avail_out;
    telnetpp::u8stream received;

    std::for_each(buffer, buffer + amount_received,
        [&codec, &received](telnetpp::u8 byte)
        {
            telnetpp::u8stream partial = codec.receive(byte);
            received.insert(received.end(), partial.begin(), partial.end());
        });

    ASSERT_EQ(expected, received);
}