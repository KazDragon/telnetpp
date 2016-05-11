#include "telnetpp/options/mccp/codec.hpp"
#include "telnetpp/options/mccp/compressor.hpp"
#include "telnetpp/options/mccp/decompressor.hpp"
#include "telnetpp/options/mccp/mccp.hpp"
#include <functional>
#include <numeric>
#include <vector>
#include <zlib.h>

namespace telnetpp { namespace options { namespace mccp {

namespace {

class output_token_visitor : public boost::static_visitor<>
{
public :
    output_token_visitor(
        std::vector<telnetpp::stream_token> &tokens,
        compressor &co,
        bool &compressed,
        bool &decompressed)
      : tokens_(tokens),
        compressor_(co),
        compressed_(compressed),
        decompressed_(decompressed)
    {
    }

    void operator()(boost::any const &any)
    {
        if (any.type() == typeid(end_compression))
        {
            if (compressed_)
            {
                tokens_.push_back(compressor_.end_compression());
                compressed_ = false;
            }
        }
        else if (any.type() == typeid(begin_compression))
        {
            compressed_ = true;
        }
        else if (any.type() == typeid(end_decompression))
        {
        }
        else if (any.type() == typeid(begin_decompression))
        {
            decompressed_ = true;
        }
        else
        {
            tokens_.push_back(any);
        }
    }

    void operator()(telnetpp::u8stream const &stream)
    {
        if (compressed_)
        {
            tokens_.push_back(compressor_.compress(stream));
        }
        else
        {
            tokens_.push_back(stream);
        }
    }

private :
    std::vector<telnetpp::stream_token> &tokens_;
    compressor &compressor_;
    bool &compressed_;
    bool &decompressed_;
};

}

struct codec::impl
{
    std::shared_ptr<compressor>   compressor_;
    std::shared_ptr<decompressor> decompressor_;

    bool compressed_ = false;
    bool decompressed_ = false;
};

codec::codec(
    std::shared_ptr<compressor> const &co,
    std::shared_ptr<decompressor> const &dec)
  : pimpl_(std::make_shared<impl>())
{
    pimpl_->compressor_ = co;
    pimpl_->decompressor_ = dec;
}

codec::~codec()
{
}

std::vector<telnetpp::stream_token> codec::send(
    std::vector<telnetpp::stream_token> const &tokens)
{
    auto result = std::vector<telnetpp::stream_token>{};
    output_token_visitor visitor(
        result,
        *pimpl_->compressor_,
        pimpl_->compressed_,
        pimpl_->decompressed_);

    for (auto const &token : tokens)
    {
        boost::apply_visitor(visitor, token);
    }

    return result;
}

telnetpp::u8stream codec::receive(telnetpp::u8 byte)
{
    if (pimpl_->decompressed_)
    {
        auto const &result = pimpl_->decompressor_->decompress(byte);
        return std::get<0>(result);
    }
    else
    {
        return { byte };
    }
}

}}}
