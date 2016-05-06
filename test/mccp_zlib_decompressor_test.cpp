#include <telnetpp/options/mccp/zlib/decompressor.hpp>
#include <gtest/gtest.h>
#include <zlib.h>
#include <algorithm>
#include <random>

TEST(mccp_zlib_decompressor_test, zlib_decompressor_is_decompressor)
{
    // This MCCP codec will rely on the decompressor interface, so
    // zlib_decompressor must implement that.
    telnetpp::options::mccp::zlib::decompressor zlib_decompressor;
    telnetpp::options::mccp::decompressor &decompressor = zlib_decompressor;
}

TEST(mccp_zlib_decompressor_test, decompressing_data_returns_decompressed_data)
{
    // As we pass a compressed byte stream through the decompressor, it
    // must return byte sequences which, when concatenated, are equal to the
    // original datastream.  This is a little bit white-box, so it's not
    // possible to know which byte sequences are returned from which byte; 
    // just what the sum total of the operation should be.
    telnetpp::options::mccp::zlib::decompressor zlib_decompressor;
    telnetpp::options::mccp::decompressor &decompressor = zlib_decompressor;
    
    auto const expected = telnetpp::u8stream {
        'd', 'a', 't', 'a', 'd', 'a', 't', 'a', 'd', 'a', 't', 'a',
        'd', 'a', 't', 'a', 'd', 'a', 't', 'a', 'd', 'a', 't', 'a',
    };
    
    z_stream stream = {};
    auto response = deflateInit(&stream, Z_DEFAULT_COMPRESSION);
    assert(response == Z_OK);
    
    telnetpp::u8 output_buffer[1023];
    
    stream.avail_in  = expected.size();
    stream.next_in   = const_cast<telnetpp::u8 *>(&expected[0]);
    stream.avail_out = sizeof(output_buffer);
    stream.next_out  = output_buffer;
    
    response = deflate(&stream, Z_SYNC_FLUSH);
    assert(response == Z_OK);
    
    telnetpp::u8stream const compressed_stream(output_buffer, stream.next_out);
    telnetpp::u8stream reassembled_stream;
    
    for (auto byte : compressed_stream)
    {
        auto result = decompressor.decompress(byte);
        
        // The stream does not end during this operation.
        ASSERT_EQ(false, std::get<1>(result));
        
        auto const &partial_stream = std::get<0>(result);
        
        // Append the resultant stream to the reassembled stream.
        reassembled_stream.insert(
            reassembled_stream.end(),
            partial_stream.begin(),
            partial_stream.end());
    }
    
    ASSERT_EQ(expected, reassembled_stream);
    
    response = deflateEnd(&stream);
    assert(response == Z_DATA_ERROR);
}

TEST(mccp_zlib_decompressor_test, decompressing_the_end_of_the_stream_returns_true)
{
    // At the end of a decompression stream, the boolean member of the tuple
    // must be set to true.  The data should also still be set as it was above.
    telnetpp::options::mccp::zlib::decompressor zlib_decompressor;
    telnetpp::options::mccp::decompressor &decompressor = zlib_decompressor;
    
    auto const expected = telnetpp::u8stream {
        'd', 'a', 't', 'a', 'd', 'a', 't', 'a', 'd', 'a', 't', 'a',
        'd', 'a', 't', 'a', 'd', 'a', 't', 'a', 'd', 'a', 't', 'a',
    };
    
    z_stream stream = {};
    auto response = deflateInit(&stream, Z_DEFAULT_COMPRESSION);
    assert(response == Z_OK);
    
    telnetpp::u8 output_buffer[1023];
    
    stream.avail_in  = expected.size();
    stream.next_in   = const_cast<telnetpp::u8 *>(&expected[0]);
    stream.avail_out = sizeof(output_buffer);
    stream.next_out  = output_buffer;
    
    response = deflate(&stream, Z_FINISH);
    assert(response == Z_STREAM_END);
    
    telnetpp::u8stream const compressed_stream(output_buffer, stream.next_out);
    telnetpp::u8stream reassembled_stream;
    
    bool last_iteration_was_end_of_stream = false;
    
    for (auto byte : compressed_stream)
    {
        // At no point should we see that the last iteration was the end of
        // the stream.
        ASSERT_EQ(false, last_iteration_was_end_of_stream);
        
        auto result = decompressor.decompress(byte);
        
        // The stream does not end during this operation.
        last_iteration_was_end_of_stream = std::get<1>(result);
        
        auto const &partial_stream = std::get<0>(result);
        
        // Append the resultant stream to the reassembled stream.
        reassembled_stream.insert(
            reassembled_stream.end(),
            partial_stream.begin(),
            partial_stream.end());
    }
    
    // The very last iteration through the stream should have set the flag.
    ASSERT_EQ(true, last_iteration_was_end_of_stream);
    
    // The stream should still have been reassembled correctly.
    ASSERT_EQ(expected, reassembled_stream);

    response = deflateEnd(&stream);
    assert(response == Z_OK);
}

TEST(mccp_zlib_decompressor_test, restarting_decompression_acts_as_if_a_new_stream)
{
    // If we've received and end of stream notification, then the next byte
    // received should work as if it were an entirely new stream.  We can 
    // verify this by compressing an entirely new stream and checking that it
    // is reassembled correctly.
    telnetpp::options::mccp::zlib::decompressor zlib_decompressor;
    telnetpp::options::mccp::decompressor &decompressor = zlib_decompressor;
    
    auto const expected = telnetpp::u8stream {
        'd', 'a', 't', 'a', 'd', 'a', 't', 'a', 'd', 'a', 't', 'a',
        'd', 'a', 't', 'a', 'd', 'a', 't', 'a', 'd', 'a', 't', 'a',
    };
    
    z_stream stream = {};
    auto response = deflateInit(&stream, Z_DEFAULT_COMPRESSION);
    assert(response == Z_OK);
    
    telnetpp::u8 output_buffer[1023];
    
    stream.avail_in  = expected.size();
    stream.next_in   = const_cast<telnetpp::u8 *>(&expected[0]);
    stream.avail_out = sizeof(output_buffer);
    stream.next_out  = output_buffer;
    
    response = deflate(&stream, Z_FINISH);
    assert(response == Z_STREAM_END);
    response = deflateEnd(&stream);
    assert(response == Z_OK);
    
    response = deflateInit(&stream, Z_DEFAULT_COMPRESSION);
    assert(response == Z_OK);
    
    stream.avail_in  = expected.size();
    stream.next_in   = const_cast<telnetpp::u8 *>(&expected[0]);
    stream.avail_out = sizeof(output_buffer);
    stream.next_out  = output_buffer;
    
    response = deflate(&stream, Z_SYNC_FLUSH);
    assert(response == Z_OK);

    telnetpp::u8stream const compressed_stream(output_buffer, stream.next_out);
    telnetpp::u8stream reassembled_stream;
    
    for (auto byte : compressed_stream)
    {
        auto result = decompressor.decompress(byte);
        
        // The stream does not end during this operation.
        ASSERT_EQ(false, std::get<1>(result));
        
        auto const &partial_stream = std::get<0>(result);
        
        // Append the resultant stream to the reassembled stream.
        reassembled_stream.insert(
            reassembled_stream.end(),
            partial_stream.begin(),
            partial_stream.end());
    }
    
    ASSERT_EQ(expected, reassembled_stream);

    response = deflateEnd(&stream);
    assert(response == Z_DATA_ERROR);
}

TEST(mccp_zlib_decompressor_test, can_decompress_large_blocks_of_data)
{
    // The decompressor should be able to decompress large blocks of data.
    // The internal buffer size is set to 1023, so decompressing a block
    // larger than that may require iterating the decompression procedure.
    // To test this, we generate a large block of random (and hence
    // uncompressable) data of around 4KB, and glue several of these together.
    telnetpp::options::mccp::zlib::decompressor zlib_decompressor;
    telnetpp::options::mccp::decompressor &decompressor = zlib_decompressor;

    static size_t const block_size = 4096;

    telnetpp::u8stream block(block_size, '\0');

    std::generate(block.begin(), block.end(),
        []{
            static std::random_device rdev;
            static std::mt19937 rng{rdev()};
            static std::uniform_int_distribution<> distribution(0, 255);
            return distribution(rng);
        });

    telnetpp::u8stream const expected = block + block + block + block;

    z_stream stream = {};
    auto response = deflateInit(&stream, Z_DEFAULT_COMPRESSION);
    assert(response == Z_OK);

    telnetpp::u8 output_buffer[block_size * 5];
    stream.next_in = const_cast<telnetpp::u8*>(&expected[0]);
    stream.avail_in = expected.size();
    stream.next_out = output_buffer;
    stream.avail_out = sizeof(output_buffer);

    response = deflate(&stream, Z_SYNC_FLUSH);
    assert(response == Z_OK);

    telnetpp::u8stream const compressed_stream(output_buffer, stream.next_out);
    telnetpp::u8stream reassembled_stream;
    
    for (auto byte : compressed_stream)
    {
        auto result = decompressor.decompress(byte);
        
        // The stream does not end during this operation.
        ASSERT_EQ(false, std::get<1>(result));
        
        auto const &partial_stream = std::get<0>(result);
        
        // Append the resultant stream to the reassembled stream.
        reassembled_stream.insert(
            reassembled_stream.end(),
            partial_stream.begin(),
            partial_stream.end());
    }
    
    ASSERT_EQ(expected, reassembled_stream);
    
    response = deflateEnd(&stream);
    assert(response == Z_DATA_ERROR);
}
