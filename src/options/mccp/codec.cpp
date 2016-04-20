#include "telnetpp/options/mccp/codec.hpp"
#include "telnetpp/options/mccp/mccp.hpp"
#include <vector>
#include <zlib.h>

namespace telnetpp { namespace options { namespace mccp {

struct codec::impl
{
    std::vector<telnetpp::stream_token> buffer_;
    z_stream                            deflate_stream_ = {};
    bool                                blocked_ = false;
    bool                                compressed_ = false;
};

namespace {

static bool is_block_token(boost::any const &any)
{
    return boost::any_cast<block_token>(&any) != nullptr;
}

static bool is_resume_uncompressed_token(boost::any const &any)
{
    return boost::any_cast<resume_uncompressed_token>(&any) != nullptr;
}

static bool is_resume_compressed_token(boost::any const &any)
{
    return boost::any_cast<resume_compressed_token>(&any) != nullptr;
}

struct token_visitor : public boost::static_visitor<>
{
public :
    token_visitor(codec::impl &impl)
      : impl_(impl)
    {
    }

    void operator()(u8stream const &elem)
    {
        if (!impl_.blocked_)
        {
            if (impl_.compressed_)
            {
                impl_.buffer_.push_back(elem);
                compress_buffer();
                append_buffer_to_result();
                clear_buffer();
            }
            else
            {
                result.push_back(elem);
            }
        }
        else
        {
            impl_.buffer_.push_back(elem);
        }
    }

    void operator()(boost::any const &any)
    {
        if (is_block_token(any))
        {
            impl_.blocked_ = true;
        }
        else if (is_resume_uncompressed_token(any))
        {
            if (impl_.compressed_)
            {
                deflateEnd(&impl_.deflate_stream_);
            }

            impl_.blocked_ = false;
            impl_.compressed_ = false;
            append_buffer_to_result();
            clear_buffer();
        }
        else if (is_resume_compressed_token(any))
        {
            if (!impl_.compressed_)
            {
                deflateInit(&impl_.deflate_stream_, Z_DEFAULT_COMPRESSION);
            }

            impl_.blocked_ = false;
            impl_.compressed_ = true;
            compress_buffer();
            append_buffer_to_result();
            clear_buffer();
        }
        else
        {
            result.push_back(any);
        }
    }

    std::vector<stream_token> result;

private :
    void append_buffer_to_result()
    {
        result.insert(
            result.end(), impl_.buffer_.begin(), impl_.buffer_.end());
    }

    void compress_stream(telnetpp::u8stream &stream)
    {
        telnetpp::u8 compress_buffer[1023];

        // Note: const_cast is correct, since ZLib doesn't change the
        // content of the input stream.
        impl_.deflate_stream_.avail_in = stream.size();
        impl_.deflate_stream_.next_in =
            const_cast<telnetpp::u8 *>(stream.data());

        impl_.deflate_stream_.avail_out = sizeof(compress_buffer);
        impl_.deflate_stream_.next_out = compress_buffer;

        // TODO: test deflating input stream that results in output stream
        // larger than buffer size.
        // TODO: robustness for error codes.
        auto result = deflate(&impl_.deflate_stream_, Z_SYNC_FLUSH);
        assert(result == Z_OK);

        auto compress_buffer_end =
            compress_buffer
            + (sizeof(compress_buffer) - impl_.deflate_stream_.avail_out);

        stream = telnetpp::u8stream(compress_buffer, compress_buffer_end);
    }

    void compress_buffer()
    {

        for (auto &token : impl_.buffer_)
        {
            auto *stream = boost::get<telnetpp::u8stream>(&token);
            // TODO: test pass-through tokens.
            assert(stream != nullptr);

            compress_stream(*stream);
        }
    }

    void clear_buffer()
    {
        impl_.buffer_.clear();
    }

    codec::impl &impl_;
};

}

codec::codec()
  : pimpl_(std::make_shared<impl>())
{
}

codec::~codec()
{
}

std::vector<stream_token> codec::send(std::vector<stream_token> const &tokens)
{
    token_visitor visitor(*pimpl_);

    for (auto const &token : tokens)
    {
        boost::apply_visitor(visitor, token);
    }

    return visitor.result;
}

}}}