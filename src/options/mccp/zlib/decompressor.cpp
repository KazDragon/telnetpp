#include "telnetpp/options/mccp/zlib/decompressor.hpp"
#include <zlib.h>
#include <cassert>

namespace telnetpp { namespace options { namespace mccp { namespace zlib {

namespace {

// In a Telnet application, we don't expect to be receiving massive amounts
// of data at a time; usually only a few bytes.  Therefore, it's not necessary
// to allocate massive blocks of 128KB or 256KB as suggested by the ZLib
// how-to guide.  Instead, we can just use small blocks on the stack and
// iterate in the very rare case that a single message yields a block of 1KB
// or more.
static std::size_t constexpr input_buffer_size = 1023;

}

// ==========================================================================
// DECOMPRESSOR::IMPL
// ==========================================================================
class decompressor::impl
{
public :
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl() = default;

    // ======================================================================
    // COPY CONSTRUCTOR
    // ======================================================================
    impl(impl const &) = delete;

    // ======================================================================
    // COPY ASSIGNMENT
    // ======================================================================
    impl &operator=(impl const &) = delete;

    ~impl()
    {
        if (decompressing_)
        {
            auto response = inflateEnd(&stream_);
            assert(response == Z_OK);
        }
    }

    // ======================================================================
    // DECOMPRESS
    // ======================================================================
    std::tuple<telnetpp::byte_stream, bool> decompress(byte data)
    {
        if (!decompressing_)
        {
            auto result = inflateInit(&stream_);
            assert(result == Z_OK);
            
            decompressing_ = true;
        }

        std::tuple<telnetpp::byte_stream, bool> result;
        auto &decompressed_stream = std::get<0>(result);
        auto &is_end_of_stream    = std::get<1>(result);

        byte input_buffer[input_buffer_size];

        stream_.avail_in  = 1;
        stream_.next_in   = &data;
        stream_.avail_out = input_buffer_size;
        stream_.next_out  = input_buffer;

        auto response = inflate(&stream_, Z_SYNC_FLUSH);

        if (response == Z_DATA_ERROR)
        {
            throw corrupted_stream_error(
                "Inflation of byte in ZLib stream yielded Z_DATA_ERROR");
        }

        assert(response == Z_OK || response == Z_STREAM_END);

        // Error in stream yields Z_DATA_ERROR
        is_end_of_stream = response == Z_STREAM_END;

        decompressed_stream.insert(
            decompressed_stream.end(),
            input_buffer,
            stream_.next_out);

        return result;
    }

    // ======================================================================
    // END_DECOMPRESSION
    // ======================================================================
    void end_decompression()
    {
        if (decompressing_)
        {
            auto result = inflateEnd(&stream_);
            assert(result == Z_OK);

            decompressing_ = false;
        }
    }

private :
    z_stream stream_ = {};
    bool     decompressing_ = false;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
decompressor::decompressor()
  : pimpl_(new impl)
{
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
decompressor::~decompressor()
{
}

// ==========================================================================
// DECOMPRESS
// ==========================================================================
std::tuple<telnetpp::byte_stream, bool> decompressor::decompress(byte data)
{
    return pimpl_->decompress(data);
}

// ==========================================================================
// END_DECOMPRESSION
// ==========================================================================
void decompressor::end_decompression()
{
    pimpl_->end_decompression();
}

}}}}
