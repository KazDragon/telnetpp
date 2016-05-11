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
        decompressor &dec,
        bool &output_compressed,
        bool &input_compressed)
      : tokens_(tokens),
        compressor_(co),
        decompressor_(dec),
        output_compressed_(output_compressed),
        input_compressed_(input_compressed)
    {
    }

    void operator()(boost::any const &any)
    {
        if (any.type() == typeid(end_compression))
        {
            if (output_compressed_)
            {
                tokens_.push_back(compressor_.end_compression());
                output_compressed_ = false;
            }
        }
        else if (any.type() == typeid(begin_compression))
        {
            output_compressed_ = true;
        }
        else if (any.type() == typeid(end_decompression))
        {
            if (input_compressed_)
            {
                decompressor_.end_decompression();
                input_compressed_ = false;
            }
        }
        else if (any.type() == typeid(begin_decompression))
        {
            input_compressed_ = true;
        }
        else
        {
            tokens_.push_back(any);
        }
    }

    void operator()(telnetpp::u8stream const &stream)
    {
        if (output_compressed_)
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
    decompressor &decompressor_;
    bool &output_compressed_;
    bool &input_compressed_;
};

}

struct codec::impl
{
    std::shared_ptr<compressor>   compressor_;
    std::shared_ptr<decompressor> decompressor_;

    bool output_compressed_ = false;
    bool input_compressed_  = false;
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
        *pimpl_->decompressor_,
        pimpl_->output_compressed_,
        pimpl_->input_compressed_);

    for (auto const &token : tokens)
    {
        boost::apply_visitor(visitor, token);
    }

    return result;
}

telnetpp::u8stream codec::receive(telnetpp::u8 byte)
{
    if (pimpl_->input_compressed_)
    {
        auto const &result = pimpl_->decompressor_->decompress(byte);
        pimpl_->input_compressed_ = !std::get<1>(result);
        return std::get<0>(result);
    }
    else
    {
        return { byte };
    }
}

}}}
