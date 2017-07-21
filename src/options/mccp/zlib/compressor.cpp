#include "telnetpp/options/mccp/zlib/compressor.hpp"
#include <zlib.h>
#include <cassert>

namespace telnetpp { namespace options { namespace mccp { namespace zlib {

namespace {

// In a Telnet application, we don't expect to be sending out massive amounts
// of data at a time; usually only a few bytes.  Therefore, it's not necessary
// to allocate massive blocks of 128KB or 256KB as suggested by the ZLib
// how-to guide.  Instead, we can just use small blocks on the stack and
// iterate in the very rare case that a single message yields a block of 1KB
// or more.
static std::size_t constexpr output_buffer_size = 1023;

}

// ==========================================================================
// ZLIB_COMPRESSION::IMPL
// ==========================================================================
struct compressor::impl
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl()
    {
        auto result = deflateInit(&stream_, Z_DEFAULT_COMPRESSION);
        assert(result == Z_OK);
    }

    // ======================================================================
    // DESTRUCTOR
    // ======================================================================
    ~impl()
    {
        if (compression_started_)
        {
            auto result = deflateEnd(&stream_);

            // deflateEnd may return Z_OK if everything was cleaned up
            // properly.  However, if the stream is interrupted without
            // finishing -- i.e. end_compression is not called, then it
            // returns Z_DATA_ERROR.  This is not an error for us.
            assert(result == Z_OK || result == Z_DATA_ERROR);
        }
    }

    // ======================================================================
    // COMPRESS
    // ======================================================================
    telnetpp::byte_stream compress(telnetpp::byte_stream const &sequence)
    {
        if (!compression_started_)
        {
            auto result = deflateInit(&stream_, Z_DEFAULT_COMPRESSION);
            assert(result == Z_OK);
            compression_started_ = true;
        }

        telnetpp::byte_stream compressed_data;

        stream_.avail_in  = sequence.size();
        stream_.next_in   = const_cast<byte *>(sequence.data());

        do
        {
            byte output_buffer[output_buffer_size];

            stream_.avail_out = output_buffer_size;
            stream_.next_out  = output_buffer;

            auto result = deflate(&stream_, Z_SYNC_FLUSH);
            assert(result == Z_OK);

            compressed_data.insert(
                compressed_data.end(),
                output_buffer,
                stream_.next_out);
        } while (stream_.avail_out == 0);

        return compressed_data;
    }

    // ======================================================================
    // END_COMPRESSION
    // ======================================================================
    telnetpp::byte_stream end_compression()
    {
        byte input_buffer[1] = {};
        byte output_buffer[output_buffer_size];

        stream_.avail_in  = 0;
        stream_.next_in   = input_buffer;
        stream_.avail_out = output_buffer_size;
        stream_.next_out  = output_buffer;

        auto result = deflate(&stream_, Z_FINISH);
        assert(result == Z_STREAM_END);

        auto const response =
            telnetpp::byte_stream(output_buffer, stream_.next_out);

        result = deflateEnd(&stream_);
        assert(result == Z_OK);
        compression_started_ = false;

        return response;
    }

    bool     compression_started_ = true;
    z_stream stream_ = {};
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
compressor::compressor()
  : pimpl_(new impl)
{
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
compressor::~compressor()
{
}

// ==========================================================================
// COMPRESS
// ==========================================================================
telnetpp::byte_stream compressor::compress(
    telnetpp::byte_stream const &sequence)
{
    if (sequence.empty())
    {
        return {};
    }
    else
    {
        return pimpl_->compress(sequence);
    }
}

// ==========================================================================
// END_COMPRESSION
// ==========================================================================
telnetpp::byte_stream compressor::end_compression()
{
    return pimpl_->end_compression();
}


}}}}
