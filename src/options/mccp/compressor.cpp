#include "telnetpp/options/mccp/compressor.hpp"
#include "telnetpp/options/mccp/mccp.hpp"
#include <functional>
#include <vector>
#include <zlib.h>

namespace telnetpp { namespace options { namespace mccp {

namespace {

struct token_visitor : public boost::static_visitor<>
{
    token_visitor(
        std::function<void (telnetpp::u8stream const &)> const &on_stream,
        std::function<void (boost::any const &)> const &on_any)
      : on_stream_(on_stream),
        on_any_(on_any)
    {
    }

    void operator()(u8stream const &elem)
    {
        on_stream_(elem);
    }

    void operator()(boost::any const &any)
    {
        on_any_(any);
    }

    std::function<void (telnetpp::u8stream const &)> on_stream_;
    std::function<void (boost::any const &)> on_any_;
};

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

}

struct compressor::impl
{
    ~impl()
    {
        end_compression();
    }

    std::vector<telnetpp::stream_token> send(
        std::vector<telnetpp::stream_token> const &tokens)
    {
        result_ = {};

        token_visitor visitor(
            [this](auto const &elem){handle_stream(elem);},
            [this](auto const &any){handle_any(any);});

        for (auto const &token : tokens)
        {
            boost::apply_visitor(visitor, token);
        }

        return result_;
    }

private :
    void block_buffer()
    {
        blocked_ = true;
    }

    void unblock_buffer()
    {
        blocked_ = false;
        append_buffer_to_result();
        clear_buffer();
    }

    void begin_compression()
    {
        if (!compressed_)
        {
            deflateInit(&deflate_stream_, Z_DEFAULT_COMPRESSION);
            compressed_ = true;
        }
    }

    void end_compression()
    {
        if (compressed_)
        {
            deflateEnd(&deflate_stream_);
            compressed_ = false;
        }
    }

    void handle_stream(u8stream const &elem)
    {
        if (!blocked_)
        {
            if (compressed_)
            {
                buffer_.push_back(elem);
                compress_buffer();
                append_buffer_to_result();
                clear_buffer();
            }
            else
            {
                result_.push_back(elem);
            }
        }
        else
        {
            buffer_.push_back(elem);
        }
    }

    void handle_any(boost::any const &any)
    {
        if (is_block_token(any))
        {
            block_buffer();
        }
        else if (is_resume_uncompressed_token(any))
        {
            end_compression();
            unblock_buffer();
        }
        else if (is_resume_compressed_token(any))
        {
            begin_compression();
            compress_buffer();
            unblock_buffer();
        }
        else
        {
            result_.push_back(any);
        }
    }

    void append_buffer_to_result()
    {
        result_.insert(
            result_.end(), buffer_.begin(), buffer_.end());
    }

    void compress_stream(telnetpp::u8stream &stream)
    {
        telnetpp::u8 compress_buffer[1023];
        telnetpp::u8stream output;

        deflate_stream_.avail_in = stream.size();
        deflate_stream_.next_in = &stream[0];

        auto result = Z_OK;

        do
        {
            deflate_stream_.avail_out = sizeof(compress_buffer);
            deflate_stream_.next_out = compress_buffer;

            // TODO: robustness for error codes.
            result = deflate(&deflate_stream_, Z_SYNC_FLUSH);
            assert(result == Z_OK);

            auto compress_buffer_end =
                compress_buffer
                + (sizeof(compress_buffer) - deflate_stream_.avail_out);

            output.insert(output.end(), compress_buffer, compress_buffer_end);
        } while (result == Z_OK && deflate_stream_.avail_out == 0);

        std::swap(stream, output);
    }

    void compress_buffer()
    {
        for (auto &token : buffer_)
        {
            auto *stream = boost::get<telnetpp::u8stream>(&token);
            // TODO: test pass-through tokens.
            assert(stream != nullptr);

            compress_stream(*stream);
        }
    }

    void clear_buffer()
    {
        buffer_.clear();
    }

    std::vector<telnetpp::stream_token> buffer_;
    std::vector<telnetpp::stream_token> result_;
    z_stream                            deflate_stream_ = {};
    bool                                blocked_ = false;
    bool                                compressed_ = false;
};

compressor::compressor()
  : pimpl_(std::make_shared<impl>())
{
}

compressor::~compressor()
{
}

std::vector<stream_token> compressor::send(std::vector<stream_token> const &tokens)
{
    return pimpl_->send(tokens);
}

}}}