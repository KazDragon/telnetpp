#include "telnetpp/options/mccp/codec.hpp"
#include "telnetpp/options/mccp/mccp.hpp"
#include <functional>
#include <numeric>
#include <vector>
#include <zlib.h>

namespace telnetpp { namespace options { namespace mccp {

namespace {

static bool is_begin_compression_token(boost::any const &any)
{
    return boost::any_cast<telnetpp::options::mccp::begin_compression>(&any) != nullptr;
}

static bool is_end_compression_token(boost::any const &any)
{
    return boost::any_cast<telnetpp::options::mccp::end_compression>(&any) != nullptr;
}

static bool is_begin_decompression_token(boost::any const &any)
{
    return boost::any_cast<telnetpp::options::mccp::begin_decompression>(&any) != nullptr;
}

static bool is_end_decompression_token(boost::any const &any)
{
    return boost::any_cast<telnetpp::options::mccp::end_decompression>(&any) != nullptr;
}

static telnetpp::u8stream compress(
    z_stream &stream, telnetpp::u8stream const &data)
{
    telnetpp::u8       buffer[1023];
    telnetpp::u8stream output;

    // Note: const_cast is correct, since this doesn't actually modify the
    // underlying data.
    stream.avail_in = data.size();
    stream.next_in  = &const_cast<telnetpp::u8stream &>(data)[0];

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

    return output;
}

static telnetpp::u8stream compress_final(z_stream &stream)
{
    telnetpp::u8       input[1] = {};
    telnetpp::u8       buffer[1023];
    telnetpp::u8stream output;

    stream.avail_in  = 0;
    stream.next_in   = input;
    stream.avail_out = sizeof(buffer);
    stream.next_out  = buffer;

    auto result = deflate(&stream, Z_FINISH);
    assert(result == Z_STREAM_END);

    auto amount_compressed =
        sizeof(buffer) - stream.avail_out;
    auto buffer_end = buffer + amount_compressed;

    output.insert(output.end(), buffer, buffer_end);
    return output;
}

class token_visitor
  : public boost::static_visitor<std::vector<telnetpp::stream_token>>
{
public :
    token_visitor(
        z_stream &input_stream,
        z_stream &output_stream,
        bool &input_compressed,
        bool &output_compressed)
      : input_stream_(input_stream),
        output_stream_(output_stream),
        input_compressed_(input_compressed),
        output_compressed_(output_compressed)
    {
    }

    std::vector<telnetpp::stream_token> operator()(
        telnetpp::u8stream const &uncompressed_data)
    {
        if (output_compressed_)
        {
            return { compress(output_stream_, uncompressed_data) };
        }
        else
        {
            return { uncompressed_data };
        }
    }

    std::vector<telnetpp::stream_token> operator()(boost::any const &any)
    {
        if (is_begin_compression_token(any))
        {
            return begin_compression();
        }
        else if (is_end_compression_token(any))
        {
            return end_compression();
        }
        else if (is_begin_decompression_token(any))
        {
            begin_decompression();
            return {};
        }
        else if (is_end_decompression_token(any))
        {
            end_decompression();
            return {};
        }
        else
        {
            return { any };
        }
    }

private :
    std::vector<telnetpp::stream_token> begin_compression()
    {
        if (output_compressed_)
        {
            deflateEnd(&output_stream_);
        }

        deflateInit(&output_stream_, Z_DEFAULT_COMPRESSION);
        output_compressed_ = true;
        return {};
    }

    std::vector<telnetpp::stream_token> end_compression()
    {
        if (output_compressed_)
        {
            auto result = compress_final(output_stream_);
            deflateEnd(&output_stream_);
            output_compressed_ = false;

            return { result };
        }
        else
        {
            return {};
        }
    }

    void begin_decompression()
    {
        if (input_compressed_)
        {
            inflateEnd(&input_stream_);
        }

        inflateInit(&input_stream_);
        input_compressed_ = true;
    }

    void end_decompression()
    {
        if (input_compressed_)
        {
            inflateEnd(&input_stream_);
            input_compressed_ = false;
        }
    }

    z_stream &input_stream_;
    z_stream &output_stream_;
    bool     &input_compressed_;
    bool     &output_compressed_;
};

class decompressor
{
public :
    decompressor(z_stream &stream, bool &compressed)
      : stream_(stream),
        compressed_(compressed)
    {
    }

    telnetpp::u8stream operator()(telnetpp::u8 byte)
    {
        if (compressed_)
        {
            telnetpp::u8 buffer[1024];

            stream_.next_in   = &byte;
            stream_.avail_in  = 1;
            stream_.next_out  = buffer;
            stream_.avail_out = sizeof(buffer);

            auto result = inflate(&stream_, Z_SYNC_FLUSH);
            assert(result == Z_OK || result == Z_STREAM_END);

            if (result == Z_STREAM_END)
            {
                end_compression();
            }

            auto amount_decompressed = sizeof(buffer) - stream_.avail_out;
            return telnetpp::u8stream(buffer, amount_decompressed);
        }
        else
        {
            return { byte };
        }
    }

private :
    void end_compression()
    {
        inflateEnd(&stream_);
        compressed_ = false;
    }

    z_stream &stream_;
    bool &compressed_;
};

}

struct codec::impl
{
    ~impl()
    {
        if (input_compressed_)
        {
            inflateEnd(&input_stream_);
        }

        if (output_compressed_)
        {
            deflateEnd(&output_stream_);
        }
    }

    z_stream input_stream_  = {};
    z_stream output_stream_ = {};
    bool input_compressed_  = false;
    bool output_compressed_ = false;
};

codec::codec()
  : pimpl_(std::make_shared<impl>())
{
}

codec::~codec()
{
}

std::vector<telnetpp::stream_token> codec::send(
    std::vector<telnetpp::stream_token> const &tokens)
{
    auto visitor = token_visitor(
        pimpl_->input_stream_,
        pimpl_->output_stream_,
        pimpl_->input_compressed_,
        pimpl_->output_compressed_);

    auto result = std::accumulate(
        tokens.begin(),
        tokens.end(),
        std::vector<telnetpp::stream_token>{},
        [&visitor](auto &result, auto const &token)
        {
            auto const compressed_tokens =
                boost::apply_visitor(visitor, token);

            result.insert(
                result.end(),
                compressed_tokens.begin(),
                compressed_tokens.end());

            return result;
        });

    return result;
}

telnetpp::u8stream codec::receive(telnetpp::u8 byte)
{
    decompressor dec(
        pimpl_->input_stream_, pimpl_->input_compressed_);
    return dec(byte);
}

}}}
