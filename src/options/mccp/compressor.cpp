#include "telnetpp/options/mccp/compressor.hpp"
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
    token_visitor(z_stream &stream, bool &compressed)
      : stream_(stream),
        compressed_(compressed)
    {
    }
    
    std::vector<telnetpp::stream_token> operator()(
        telnetpp::u8stream const &uncompressed_data)
    {
        if (compressed_)
        {
            return { compress(stream_, uncompressed_data) };
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
        else
        {
            return { any };
        }
    }
    
private :
    std::vector<telnetpp::stream_token> begin_compression()
    {
        if (compressed_)
        {
            deflateEnd(&stream_);
        }
        
        deflateInit(&stream_, Z_DEFAULT_COMPRESSION);
        compressed_ = true;
        return {};
    }
    
    std::vector<telnetpp::stream_token> end_compression()
    {
        if (compressed_)
        {
            auto result = compress_final(stream_);
            deflateEnd(&stream_);
            compressed_ = false;
            
            return { result };
        }
        else 
        {
            return {};
        }
    }
    
    z_stream &stream_;
    bool     &compressed_;
};

}

struct compressor::impl
{
    ~impl()
    {
        if (compressed_)
        {
            deflateEnd(&stream_);
        }
    }
    
    z_stream stream_ = {};
    bool compressed_ = false;
};

compressor::compressor()
  : pimpl_(std::make_shared<impl>())
{
}

compressor::~compressor()
{
}

std::vector<telnetpp::stream_token> compressor::send(
    std::vector<telnetpp::stream_token> const &tokens)
{
    auto visitor = token_visitor(
        pimpl_->stream_, pimpl_->compressed_);
    
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

}}}
