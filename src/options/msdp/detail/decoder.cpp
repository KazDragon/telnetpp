#include "telnetpp/options/msdp/detail/decoder.hpp"
#include <algorithm>
#include <cassert>

namespace telnetpp { namespace options { namespace msdp { namespace detail {

namespace {

using variable = telnetpp::options::msdp::variable;

class parser
{
public :
    parser()
    {
        stack_.push_back(&result_);
    }

    void operator()(telnetpp::u8 byte)
    {
        assert(!stack_.empty());

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

            case state::array:
                parse_array(byte);
                break;

            default:
                assert(!"invalid state");
                break;
        }

        assert(!stack_.empty());
    }

    std::vector<variable> result() const
    {
        return result_;
    }

private :
    auto &top()
    {
        return *stack_.back();
    }

    auto &value()
    {
        return top().back().value;
    }

    auto &value_as_string()
    {
        return boost::get<std::string>(value());
    }

    auto &value_as_array()
    {
        return boost::get<std::vector<std::string>>(value());
    }

    auto &value_as_table()
    {
        return boost::get<std::vector<variable>>(value());
    }

    auto &name()
    {
        return top().back().name;
    }

    void parse_idle(u8 byte)
    {
        if (byte == telnetpp::options::msdp::var)
        {
            top().push_back({});
            state_ = state::name;
        }
    }

    void parse_name(u8 byte)
    {
        assert(!result_.empty());

        if (byte == telnetpp::options::msdp::val)
        {
            state_ = state::value;
            value() = std::string{};
        }
        else
        {
            name() += char(byte);
        }
    }

    void parse_value(u8 byte)
    {
        assert(!result_.empty());

        switch(byte)
        {
            case telnetpp::options::msdp::var :
                top().push_back({});
                state_ = state::name;
                break;

            case telnetpp::options::msdp::table_open :
                value() = std::vector<variable>{};
                stack_.push_back(&value_as_table());
                state_ = state::idle;
                break;

            case telnetpp::options::msdp::table_close :
                // Never pop the root of the stack.
                if (stack_.size() > 1)
                {
                    stack_.pop_back();
                }
                state_ = state::idle;
                break;

            case telnetpp::options::msdp::array_open :
                value() = std::vector<std::string>{};
                state_ = state::array;
                break;

            default :
                value_as_string() += char(byte);
                break;
        }
    }

    void parse_array(u8 byte)
    {
        switch(byte)
        {
            case telnetpp::options::msdp::array_close :
                state_ = state::idle;
                break;

            case telnetpp::options::msdp::val :
                value_as_array().push_back({});
                break;

            default :
                value_as_array().back() += char(byte);
                break;
        }
    }

    enum class state
    {
        idle,
        name,
        value,
        array,
    };

    std::vector<std::vector<variable> *> stack_;
    std::vector<variable>                result_;
    state                                state_ = state::idle;
};

}

std::vector<telnetpp::options::msdp::variable> decode(
    telnetpp::u8stream const &stream)
{
    using std::begin;
    using std::end;

    parser parse;

    std::for_each(begin(stream), end(stream), std::ref(parse));

    return parse.result();
}

}}}}
