#include "telnetpp/options/msdp/detail/decoder.hpp"
#include "telnetpp/options/msdp/detail/protocol.hpp"
#include <algorithm>
#include <cassert>

namespace telnetpp { namespace options { namespace msdp { namespace detail {

namespace {

using variable = telnetpp::options::msdp::variable;

class parser
{
public :
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    parser()
    {
        stack_.push_back(&result_);
    }

    // ======================================================================
    // OPERATOR()
    // ======================================================================
    void operator()(byte data)
    {
        assert(!stack_.empty());

        switch (state_)
        {
            case state::idle:
                parse_idle(data);
                break;

            case state::name:
                parse_name(data);
                break;

            case state::value:
                parse_value(data);
                break;

            case state::array:
                parse_array(data);
                break;

            default:
                assert(!"invalid state");
                break;
        }

        assert(!stack_.empty());
    }

    // ======================================================================
    // RESULT
    // ======================================================================
    std::vector<variable> result() const
    {
        return result_;
    }

private :
    // ======================================================================
    // TOP
    // ======================================================================
    auto &top()
    {
        return *stack_.back();
    }

    // ======================================================================
    // VALUE
    // ======================================================================
    auto &value()
    {
        return top().back().value;
    }

    // ======================================================================
    // VALUE_AS_STRING
    // ======================================================================
    auto &value_as_string()
    {
        return boost::get<std::string>(value());
    }

    // ======================================================================
    // VALUE_AS_ARRAY
    // ======================================================================
    auto &value_as_array()
    {
        return boost::get<std::vector<std::string>>(value());
    }

    // ======================================================================
    // VALUE_AS_TABLE
    // ======================================================================
    auto &value_as_table()
    {
        return boost::get<std::vector<variable>>(value());
    }

    // ======================================================================
    // NAME
    // ======================================================================
    auto &name()
    {
        return top().back().name;
    }

    // ======================================================================
    // PARSE_IDLE
    // ======================================================================
    void parse_idle(byte data)
    {
        if (data == telnetpp::options::msdp::detail::var)
        {
            top().push_back({});
            state_ = state::name;
        }
    }

    // ======================================================================
    // PARSE_NAME
    // ======================================================================
    void parse_name(byte data)
    {
        assert(!result_.empty());

        if (data == telnetpp::options::msdp::detail::val)
        {
            state_ = state::value;
            value() = std::string{};
        }
        else
        {
            name() += char(data);
        }
    }

    // ======================================================================
    // PARSE_VALUE
    // ======================================================================
    void parse_value(byte data)
    {
        assert(!result_.empty());

        switch (data)
        {
            case telnetpp::options::msdp::detail::var :
                top().push_back({});
                state_ = state::name;
                break;

            case telnetpp::options::msdp::detail::table_open :
                value() = std::vector<variable>{};
                stack_.push_back(&value_as_table());
                state_ = state::idle;
                break;

            case telnetpp::options::msdp::detail::table_close :
                // Never pop the root of the stack.
                if (stack_.size() > 1)
                {
                    stack_.pop_back();
                }
                state_ = state::idle;
                break;

            case telnetpp::options::msdp::detail::array_open :
                value() = std::vector<std::string>{};
                state_ = state::array;
                break;

            default :
                value_as_string() += char(data);
                break;
        }
    }

    // ======================================================================
    // PARSE_ARRAY
    // ======================================================================
    void parse_array(byte data)
    {
        switch (data)
        {
            case telnetpp::options::msdp::detail::array_close :
                state_ = state::idle;
                break;

            case telnetpp::options::msdp::detail::val :
                value_as_array().push_back({});
                break;

            default :
                value_as_array().back() += char(data);
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

// ==========================================================================
// DECODE
// ==========================================================================
std::vector<variable> decode(telnetpp::byte_stream const &stream)
{
    using std::begin;
    using std::end;

    parser parse;

    std::for_each(begin(stream), end(stream), std::ref(parse));

    return parse.result();
}

}}}}
