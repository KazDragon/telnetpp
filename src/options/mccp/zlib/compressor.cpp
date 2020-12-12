#include "telnetpp/options/mccp/zlib/compressor.hpp"
#include <boost/optional.hpp>
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
static std::size_t constexpr output_buffer_size = 1024;

}

// ==========================================================================
// ZLIB_COMPRESSION::IMPL
// ==========================================================================
struct compressor::impl
{
    boost::optional<z_stream> stream;

    // ======================================================================
    // CONSTRUCT_STREAM
    // ======================================================================
    void construct_stream()
    {
        assert(stream == boost::none);

        stream = z_stream{};

        auto result = deflateInit(stream.get_ptr(), Z_DEFAULT_COMPRESSION);
        assert(result == Z_OK);
    }

    // ======================================================================
    // DESTROY_STREAM
    // ======================================================================
    void destroy_stream()
    {
        assert(stream != boost::none);

        auto result = deflateEnd(stream.get_ptr());
        assert(result == Z_OK || result == Z_STREAM_ERROR || Z_DATA_ERROR);

        stream = boost::none;
    }
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
    if (pimpl_->stream)
    {
        pimpl_->destroy_stream();
    }
}

// ==========================================================================
// DO_START
// ==========================================================================
void compressor::do_start()
{
}

// ==========================================================================
// DO_FINISH
// ==========================================================================
void compressor::do_finish(continuation const &cont)
{
    if (!pimpl_->stream)
    {
        pimpl_->construct_stream();
    }

    byte output_buffer[output_buffer_size];
    pimpl_->stream->avail_in  = 0;
    pimpl_->stream->next_in   = nullptr;

    auto response = Z_OK;

    do 
    {
        pimpl_->stream->avail_out = output_buffer_size;
        pimpl_->stream->next_out  = output_buffer;
        
        response = deflate(pimpl_->stream.get_ptr(), Z_FINISH);
        
        auto const output_data = telnetpp::bytes{
            output_buffer, pimpl_->stream->next_out
        };

        cont(output_data, true);
    }
    while (response == Z_OK);

    assert(response == Z_STREAM_END);

    pimpl_->destroy_stream();
}

// ==========================================================================
// TRANSFORM_CHUNK
// ==========================================================================
telnetpp::bytes compressor::transform_chunk(
    telnetpp::bytes data,
    continuation const &cont)
{
    if (!pimpl_->stream)
    {
        pimpl_->construct_stream();
    }

    byte output_buffer[output_buffer_size];
    pimpl_->stream->avail_in  = static_cast<uInt>(data.size());
    pimpl_->stream->next_in   = const_cast<telnetpp::byte *>(data.data());
    pimpl_->stream->avail_out = output_buffer_size;
    pimpl_->stream->next_out  = output_buffer;

    auto response = deflate(pimpl_->stream.get_ptr(), Z_SYNC_FLUSH);
    assert(response == Z_OK);

    auto const output_data = telnetpp::bytes{
        output_buffer, pimpl_->stream->next_out
    };

    cont(output_data, false);

    return data.subspan(data.size() - pimpl_->stream->avail_in);
}

}}}}
