#include <telnetpp/options/mccp/zlib/compressor.hpp>
#include <gtest/gtest.h>
#include <boost/make_unique.hpp>
#include <boost/range/algorithm/generate.hpp>
#include <random>
#include <zlib.h>
#include <vector>

using namespace telnetpp::literals;

namespace {

class a_zlib_compressor : public testing::Test
{
protected:
    telnetpp::options::mccp::zlib::compressor zlib_compressor_;
};

}

TEST_F(a_zlib_compressor, is_a_codec)
{
    // MCCP will rely on the codec interface, so zlib_decompressor must
    // implement that.
    telnetpp::options::mccp::codec &compressor = zlib_compressor_;
}

namespace {

class an_unstarted_zlib_compressor : public a_zlib_compressor
{
protected:
    void compress_data(telnetpp::bytes data)
    {
        zlib_compressor_(
            data,
            [&](telnetpp::bytes data, bool compression_ended)
            {
                compression_ended_ = compression_ended;
                received_data_.insert(
                    received_data_.end(), data.begin(), data.end());
            });
    }

    std::vector<telnetpp::byte> received_data_;
    bool compression_ended_ = false;
};

}

TEST_F(an_unstarted_zlib_compressor, does_not_compress_data)
{
    auto const data = "test_data"_tb;

    compress_data(data);

    auto const expected_data = std::vector<telnetpp::byte>{
        data.begin(), data.end()
    };

    ASSERT_EQ(expected_data, received_data_);
    ASSERT_FALSE(compression_ended_);
}

namespace {

class a_started_zlib_compressor : public an_unstarted_zlib_compressor
{
protected:
    a_started_zlib_compressor()
    {
        zlib_compressor_.start();
    }

    void finish_compression()
    {
        zlib_compressor_.finish(
            [&](telnetpp::bytes data, bool compression_ended)
            {
                received_data_.insert(
                    received_data_.end(),
                    data.begin(),
                    data.end());
                compression_ended_ = compression_ended;
            });
    }
};

}

TEST_F(a_started_zlib_compressor, compresses_received_data)
{
    auto const test_data = "datadatadatadatadatadata"_tb;

    compress_data(test_data);

    // Since it would be very white-box to work out what the result of the
    // compression operation is and compare to that (and perhaps also brittle
    // since data can be compressed in many ways that may change due to 
    // algorithmic improvements), instead we say that whatever has been 
    // compressed can be round-tripped via decompression to the same result.
    z_stream stream = {};
    auto response = inflateInit(&stream);
    assert(response == Z_OK);

    telnetpp::byte output_buffer[1024];
    stream.avail_in  = received_data_.size();
    stream.next_in   = const_cast<telnetpp::byte *>(received_data_.data());
    stream.avail_out = sizeof(output_buffer);
    stream.next_out  = output_buffer;

    response = inflate(&stream, Z_SYNC_FLUSH);
    EXPECT_EQ(response, Z_OK);

    auto const output_data = telnetpp::bytes{
        output_buffer, stream.next_out
    };

    auto const expected_data = telnetpp::bytes{test_data};

    ASSERT_EQ(expected_data, output_data);
    ASSERT_FALSE(compression_ended_);

    inflateEnd(&stream);
}

TEST_F(a_started_zlib_compressor, sends_a_stream_end_when_compression_is_ended)
{
    auto const test_data = "datadatadatadatadatadata"_tb;

    compress_data(test_data);
    finish_compression();
    ASSERT_TRUE(compression_ended_);

    z_stream stream = {};
    auto response = inflateInit(&stream);
    assert(response == Z_OK);

    telnetpp::byte output_buffer[1024];
    stream.avail_in  = received_data_.size();
    stream.next_in   = const_cast<telnetpp::byte *>(received_data_.data());
    stream.avail_out = sizeof(output_buffer);
    stream.next_out  = output_buffer;

    response = inflate(&stream, Z_SYNC_FLUSH);
    EXPECT_EQ(response, Z_STREAM_END);

    auto const output_data = telnetpp::bytes{
        output_buffer, stream.next_out
    };

    auto const expected_data = telnetpp::bytes{test_data};

    ASSERT_EQ(expected_data, output_data);
    ASSERT_TRUE(compression_ended_);

    inflateEnd(&stream);
}

TEST_F(a_started_zlib_compressor, can_compress_large_amounts_of_data)
{
    // Fill a large buffer of data with random noise.  This ensures that it
    // is difficult to compress to less than one send buffer in size (
    // a regular pattern might well do this).
    static std::random_device rd;
    std::mt19937 gen(rd());

    constexpr auto large_array_size = 1024 * 1024;
    std::vector<telnetpp::byte> large_data(large_array_size);
    boost::generate(large_data, gen);

    compress_data(large_data);
    finish_compression();
    ASSERT_TRUE(compression_ended_);

    // Now repeatedly decompress the compressed data until we have
    // reconstituted the original large buffer.
    z_stream stream = {};
    auto response = inflateInit(&stream);
    assert(response == Z_OK);

    telnetpp::byte output_buffer[1024];
    std::vector<telnetpp::byte> decompressed_data;

    stream.avail_in  = received_data_.size();
    stream.next_in   = const_cast<telnetpp::byte *>(received_data_.data());

    while(stream.avail_in != 0)
    {
        stream.avail_out = sizeof(output_buffer);
        stream.next_out  = output_buffer;

        response = inflate(&stream, Z_SYNC_FLUSH);
        ASSERT_TRUE(response == Z_OK || response == Z_STREAM_END);

        decompressed_data.insert(
            decompressed_data.end(),
            output_buffer,
            stream.next_out);
    }

    inflateEnd(&stream);

    ASSERT_EQ(large_data.size(), decompressed_data.size());
    ASSERT_EQ(large_data, decompressed_data);
}
