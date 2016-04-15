#include "telnetpp/options/mccp/codec.hpp"
#include "telnetpp/options/mccp/mccp.hpp"

namespace telnetpp { namespace options { namespace mccp {

namespace {

static bool is_block_token(boost::any const &any)
{
    return boost::any_cast<block_token>(&any) != nullptr;
}

static bool is_resume_uncompressed_token(boost::any const &any)
{
    return boost::any_cast<resume_uncompressed_token>(&any) != nullptr;
}

struct token_visitor : public boost::static_visitor<>
{
public :
    token_visitor(bool &blocked, std::vector<token> &buffer)
      : blocked_(blocked),
        buffer_(buffer)
    {
    }

    void operator()(element const &elem)
    {
        if (!blocked_)
        {
            result.push_back(elem);
        }
        else
        {
            buffer_.push_back(elem);
        }
    }

    void operator()(boost::any const &any)
    {
        if (is_block_token(any))
        {
            blocked_ = true;
        }
        else if (is_resume_uncompressed_token(any))
        {
            blocked_ = false;
            result.insert(result.end(), buffer_.begin(), buffer_.end());
            buffer_.clear();
        }
        else
        {
            result.push_back(any);
        }
    }

    std::vector<token> result;

private :
    bool               &blocked_;
    std::vector<token> &buffer_;
};

}

std::vector<token> codec::send(std::vector<token> const &tokens)
{
    token_visitor visitor(blocked_, buffer_);

    for (auto const &token : tokens)
    {
        boost::apply_visitor(visitor, token);
    }

    return visitor.result;
}

}}}