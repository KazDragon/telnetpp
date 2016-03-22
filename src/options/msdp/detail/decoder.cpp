#include "telnetpp/options/msdp/detail/decoder.hpp"
#include <algorithm>
#include <cassert>

namespace telnetpp { namespace options { namespace msdp { namespace detail {

namespace {

class parser
{
public :
    void operator()(telnetpp::u8 byte)
    {
        switch(state_)
        {
            case state::idle:
                parse_idle(byte);
                break;

            case state::name:
                parse_name(byte);
                break;

            case state::value:
                parse_value(byte);
                break;

            default:
                assert(!"invalid state");
                break;
        }
    }

    std::vector<telnetpp::options::msdp::variable> result() const
    {
        return result_;
    }

private :
    void parse_idle(u8 byte)
    {
        if (byte == telnetpp::options::msdp::var)
        {
            result_.push_back({});
            state_ = state::name;
        }
    }

    void parse_name(u8 byte)
    {
        assert(!result_.empty());

        if (byte == telnetpp::options::msdp::val)
        {
            state_ = state::value;
            result_.back().value = std::string{};
        }
        else
        {
            result_.back().name += char(byte);
        }
    }

    void parse_value(u8 byte)
    {
        assert(!result_.empty());

        if (byte == telnetpp::options::msdp::var)
        {
            result_.push_back({});
            state_ = state::name;
        }
        else
        {
            boost::get<std::string>(result_.back().value) += char(byte);
        }
    }

    enum class state
    {
        idle,
        name,
        value
    };

    std::vector<telnetpp::options::msdp::variable> result_;
    state state_ = state::idle;
};

}

std::vector<telnetpp::options::msdp::variable> decode(
    telnetpp::u8stream const &stream)
{
    using std::begin;
    using std::end;

    parser parse;

    std::for_each(begin(stream), end(stream),
        [&parse](auto const &byte)
        {
            parse(byte);
        });

    return parse.result();
}

}}}}
