#include "telnetpp/options/mccp/compressor.hpp"
#include "telnetpp/options/mccp/mccp.hpp"
#include <functional>
#include <vector>
#include <zlib.h>

namespace telnetpp { namespace options { namespace mccp {

namespace {

struct token_visitor
  : public boost::static_visitor<std::vector<telnetpp::stream_token>>
{
    token_visitor(
        std::function<
            std::vector<telnetpp::stream_token> (telnetpp::u8stream const &)
        > const &on_stream,
        std::function<
            std::vector<telnetpp::stream_token> (boost::any const &)
        > const &on_any)
      : on_stream_(on_stream),
        on_any_(on_any)
    {
    }

    std::vector<telnetpp::stream_token> operator()(u8stream const &elem)
    {
        return on_stream_(elem);
    }

    std::vector<telnetpp::stream_token> operator()(boost::any const &any)
    {
        return on_any_(any);
    }

    std::function<
        std::vector<telnetpp::stream_token> (telnetpp::u8stream const &)
    > on_stream_;

    std::function<
        std::vector<telnetpp::stream_token> (boost::any const &)
    > on_any_;
};

static void inplace_compress(z_stream &stream, telnetpp::u8stream &data)
{
    telnetpp::u8       buffer[1023];
    telnetpp::u8stream output;

    stream.avail_in = data.size();
    stream.next_in = &data[0];

    auto result = Z_OK;

    // Compress the stream in buffer-sized chunks.
    do
    {
        stream.avail_out = sizeof(buffer);
        stream.next_out = buffer;

        // Since we are in control of the compression stream, and will
        // never send invalid data, this should always result in Z_OK.
        result = deflate(&stream, Z_SYNC_FLUSH);
        assert(result == Z_OK);

        auto amount_compressed =
            sizeof(buffer) - stream.avail_out;
        auto buffer_end = buffer + amount_compressed;

        output.insert(output.end(), buffer, buffer_end);
    } while (result == Z_OK && stream.avail_out == 0);

    std::swap(data, output);
}

void compress_tokens(
    z_stream                            &stream,
    std::vector<telnetpp::stream_token> &tokens)
{
    for (auto &token : tokens)
    {
        auto *data = boost::get<telnetpp::u8stream>(&token);

        if (data != nullptr)
        {
            inplace_compress(stream, *data);
        }
    }
}


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
    impl()
      : visitor_(
            [this](auto const &elem){return handle_stream(elem);},
            [this](auto const &any){return handle_any(any);})
    {
    }

    ~impl()
    {
        end_compression();
    }

    std::vector<telnetpp::stream_token> send(
        std::vector<telnetpp::stream_token> const &tokens)
    {
        return std::accumulate(
            tokens.begin(),
            tokens.end(),
            std::vector<telnetpp::stream_token>{},
            [&visitor = visitor_](
                auto &cumulative_result,
                auto const &token)
            {
                auto const &result = boost::apply_visitor(visitor, token);

                cumulative_result.insert(
                    cumulative_result.end(),
                    result.begin(),
                    result.end());

                return cumulative_result;
            });
    }

private :
    void block_buffer()
    {
        blocked_ = true;
    }

    void unblock_buffer()
    {
        blocked_ = false;
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

    std::vector<telnetpp::stream_token> handle_stream(u8stream const &elem)
    {
        std::vector<telnetpp::stream_token> result;

        if (!blocked_)
        {
            if (compressed_)
            {
                buffer_.push_back(elem);
                compress_tokens(deflate_stream_, buffer_);
                std::swap(result, buffer_);
            }
            else
            {
                result.push_back(elem);
            }
        }
        else
        {
            buffer_.push_back(elem);
        }

        return result;
    }

    std::vector<telnetpp::stream_token> handle_any(boost::any const &any)
    {
        std::vector<telnetpp::stream_token> result;

        if (is_block_token(any))
        {
            block_buffer();
        }
        else if (is_resume_uncompressed_token(any))
        {
            end_compression();
            unblock_buffer();
            std::swap(result, buffer_);
        }
        else if (is_resume_compressed_token(any))
        {
            begin_compression();
            compress_tokens(deflate_stream_, buffer_);
            unblock_buffer();
            std::swap(result, buffer_);
        }
        else
        {
            if (blocked_)
            {
                buffer_.push_back(any);
            }
            else
            {
                result.push_back(any);
            }
        }

        return result;
    }

    void append_buffer_to(std::vector<telnetpp::stream_token> &result)
    {
        result.insert(
            result.end(), buffer_.begin(), buffer_.end());
    }

    std::vector<telnetpp::stream_token> buffer_;
    z_stream                            deflate_stream_ = {};
    token_visitor                       visitor_;
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