#include "telnetpp/options/mccp/zlib/decompressor.hpp"

#include <boost/core/ignore_unused.hpp>
#include <zlib.h>

#include <optional>
#include <cassert>

namespace telnetpp::options::mccp::zlib {

namespace {

// In a Telnet application, we don't expect to be receiving massive amounts
// of data at a time; usually only a few bytes.  Therefore, it's not necessary
// to allocate massive blocks of 128KB or 256KB as suggested by the ZLib
// how-to guide.  Instead, we can just use small blocks on the stack and
// iterate in the very rare case that a single message yields a block of 1KB
// or more.
constexpr std::size_t receive_buffer_size = 1024;

}  // namespace

// ==========================================================================
// DECOMPRESSOR::IMPL
// ==========================================================================
struct decompressor::impl
{
    std::optional<z_stream> stream_;

    // ======================================================================
    // CONSTRUCT_STREAM
    // ======================================================================
    void construct_stream()
    {
        assert(stream_ == std::nullopt);

        stream_ = z_stream{};

        auto result = inflateInit(&*stream_);
        boost::ignore_unused(result);
        assert(result == Z_OK);
    }

    // ======================================================================
    // DESTROY_STREAM
    // ======================================================================
    void destroy_stream()
    {
        assert(stream_ != std::nullopt);

        auto result = inflateEnd(&*stream_);
        boost::ignore_unused(result);
        assert(result == Z_OK || result == Z_STREAM_ERROR);

        stream_ = std::nullopt;
    }
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
decompressor::decompressor() : pimpl_(new impl)
{
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
decompressor::~decompressor()
{
    if (pimpl_->stream_)
    {
        pimpl_->destroy_stream();
    }
}

// ==========================================================================
// DO_START
// ==========================================================================
void decompressor::do_start()
{
}

// ==========================================================================
// DO_FINISH
// ==========================================================================
void decompressor::do_finish(continuation const & /*cont*/)
{
    if (pimpl_->stream_)
    {
        pimpl_->destroy_stream();
    }
}

// ==========================================================================
// TRANSFORM_CHUNK
// ==========================================================================
telnetpp::bytes decompressor::transform_chunk(
    telnetpp::bytes data, continuation const &cont)
{
    if (!pimpl_->stream_)
    {
        pimpl_->construct_stream();
    }

    assert(pimpl_->stream_.has_value());
    byte receive_buffer[receive_buffer_size];
    pimpl_->stream_->avail_in = static_cast<uInt>(data.size());
    pimpl_->stream_->next_in = const_cast<telnetpp::byte *>(data.data());
    pimpl_->stream_->avail_out = receive_buffer_size;
    pimpl_->stream_->next_out = receive_buffer;

    auto response = inflate(&*pimpl_->stream_, Z_SYNC_FLUSH);

    if (response == Z_DATA_ERROR)
    {
        throw corrupted_stream_error(
            "Inflation of byte in ZLib stream yielded Z_DATA_ERROR");
    }

    assert(response == Z_OK || response == Z_STREAM_END);

    auto const received_data =
        telnetpp::bytes{receive_buffer, pimpl_->stream_->next_out};

    bool const stream_ended = response == Z_STREAM_END;
    data = data.subspan(data.size() - pimpl_->stream_->avail_in);

    if (stream_ended)
    {
        pimpl_->destroy_stream();
    }

    cont(received_data, stream_ended);

    return data;
}

}  // namespace telnetpp::options::mccp::zlib
