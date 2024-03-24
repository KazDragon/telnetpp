#include "telnetpp/options/mccp/zlib/compressor.hpp"

#include <boost/core/ignore_unused.hpp>
#include <zlib.h>

#include <optional>
#include <cassert>

namespace telnetpp::options::mccp::zlib {

namespace {

// In a Telnet application, we don't expect to be sending out massive amounts
// of data at a time; usually only a few bytes.  Therefore, it's not necessary
// to allocate massive blocks of 128KB or 256KB as suggested by the ZLib
// how-to guide.  Instead, we can just use small blocks on the stack and
// iterate in the very rare case that a single message yields a block of 1KB
// or more.
constexpr std::size_t output_buffer_size = 1024;

}  // namespace

// ==========================================================================
// ZLIB_COMPRESSION::IMPL
// ==========================================================================
struct compressor::impl
{
    std::optional<z_stream> stream_;

    // ======================================================================
    // CONSTRUCT_STREAM
    // ======================================================================
    void construct_stream()
    {
        assert(stream_ == std::nullopt);

        stream_ = z_stream{};

        auto result = deflateInit(&*stream_, Z_DEFAULT_COMPRESSION);
        boost::ignore_unused(result);
        assert(result == Z_OK);
    }

    // ======================================================================
    // DESTROY_STREAM
    // ======================================================================
    void destroy_stream()
    {
        assert(stream_ != std::nullopt);

        auto result = deflateEnd(&*stream_);
        boost::ignore_unused(result);
        assert(result == Z_OK || result == Z_STREAM_ERROR || Z_DATA_ERROR);

        stream_ = std::nullopt;
    }
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
compressor::compressor() : pimpl_(new impl)
{
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
compressor::~compressor()
{
    if (pimpl_->stream_)
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
    if (!pimpl_->stream_)
    {
        pimpl_->construct_stream();
    }

    assert(pimpl_->stream_.has_value());
    byte output_buffer[output_buffer_size];
    pimpl_->stream_->avail_in = 0;
    pimpl_->stream_->next_in = nullptr;

    auto response = Z_OK;

    do
    {
        pimpl_->stream_->avail_out = output_buffer_size;
        pimpl_->stream_->next_out = output_buffer;

        response = deflate(&*pimpl_->stream_, Z_FINISH);

        auto const output_data =
            telnetpp::bytes{output_buffer, pimpl_->stream_->next_out};

        cont(output_data, true);
    } while (response == Z_OK);

    assert(response == Z_STREAM_END);

    pimpl_->destroy_stream();
}

// ==========================================================================
// TRANSFORM_CHUNK
// ==========================================================================
telnetpp::bytes compressor::transform_chunk(
    telnetpp::bytes data, continuation const &cont)
{
    if (!pimpl_->stream_)
    {
        pimpl_->construct_stream();
    }

    assert(pimpl_->stream_.has_value());
    byte output_buffer[output_buffer_size];
    pimpl_->stream_->avail_in = static_cast<uInt>(data.size());
    pimpl_->stream_->next_in = const_cast<telnetpp::byte *>(data.data());
    pimpl_->stream_->avail_out = output_buffer_size;
    pimpl_->stream_->next_out = output_buffer;

    auto response = deflate(&*pimpl_->stream_, Z_SYNC_FLUSH);
    boost::ignore_unused(response);
    assert(response == Z_OK);

    auto const output_data =
        telnetpp::bytes{output_buffer, pimpl_->stream_->next_out};

    cont(output_data, false);

    return data.subspan(data.size() - pimpl_->stream_->avail_in);
}

}  // namespace telnetpp::options::mccp::zlib
