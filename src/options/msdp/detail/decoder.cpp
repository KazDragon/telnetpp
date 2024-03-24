#include "telnetpp/options/msdp/detail/decoder.hpp"

#include "telnetpp/options/msdp/detail/protocol.hpp"

#include <functional>
#include <utility>
#include <vector>
#include <cassert>

namespace telnetpp::options::msdp::detail {

namespace {

using variable = telnetpp::options::msdp::variable;

class parser
{
   public:
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    explicit parser(std::function<void(variable const &)> cont)
      : cont_(std::move(cont))
    {
    }

    // ======================================================================
    // OPERATOR()
    // ======================================================================
    void operator()(byte data)
    {
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
    }

    // ======================================================================
    // FINISH
    // ======================================================================
    void finish()
    {
        if (!var_.name_.empty())
        {
            cont_(var_);
        }
    }

   private:
    // ======================================================================
    // TOP
    // ======================================================================
    auto &top()
    {
        return *current_var_.back();
    }

    // ======================================================================
    // VALUE
    // ======================================================================
    auto &value()
    {
        return top().value_;
    }

    // ======================================================================
    // VALUE_AS_STRING
    // ======================================================================
    auto &value_as_string()
    {
        return std::get<string_value>(value());
    }

    // ======================================================================
    // VALUE_AS_ARRAY
    // ======================================================================
    auto &value_as_array()
    {
        return std::get<array_value>(value());
    }

    // ======================================================================
    // VALUE_AS_TABLE
    // ======================================================================
    auto &value_as_table()
    {
        return std::get<table_value>(value());
    }

    // ======================================================================
    // NAME
    // ======================================================================
    auto &name()
    {
        return top().name_;
    }

    // ======================================================================
    // OPEN_VARIABLE
    // ======================================================================
    void open_variable()
    {
        if (current_var_.empty())
        {
            var_ = variable{};
            current_var_.push_back(&var_);
        }
        else
        {
            auto &outer_var = value_as_table();
            outer_var.emplace_back();
            current_var_.push_back(&outer_var.back());
        }
    }

    // ======================================================================
    // CLOSE_VARIABLE
    // ======================================================================
    void close_variable()
    {
        current_var_.pop_back();

        if (current_var_.empty())
        {
            cont_(var_);
            var_ = {};
        }
    }

    // ======================================================================
    // PARSE_IDLE
    // ======================================================================
    void parse_idle(byte data)
    {
        switch (data)
        {
            case telnetpp::options::msdp::detail::var:
                open_variable();
                state_ = state::name;
                break;

            case telnetpp::options::msdp::detail::table_close:
                close_variable();
                state_ = state::idle;
                break;

            default:
                break;
        }
    }

    // ======================================================================
    // PARSE_NAME
    // ======================================================================
    void parse_name(byte data)
    {
        if (data == telnetpp::options::msdp::detail::val)
        {
            state_ = state::value;
            value() = string_value{};
        }
        else
        {
            name() += data;
        }
    }

    // ======================================================================
    // PARSE_VALUE
    // ======================================================================
    void parse_value(byte data)
    {
        switch (data)
        {
            case telnetpp::options::msdp::detail::array_open:
                value() = array_value{};
                state_ = state::array;
                break;

            case telnetpp::options::msdp::detail::table_open:
                value() = table_value{};
                state_ = state::idle;
                break;

            case telnetpp::options::msdp::detail::table_close:
                close_variable();
                state_ = state::idle;
                break;

            case telnetpp::options::msdp::detail::var:
                close_variable();
                open_variable();
                state_ = state::name;
                break;

            default:
                value_as_string() += data;
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
            case telnetpp::options::msdp::detail::array_close:
                close_variable();
                state_ = state::idle;
                break;

            case telnetpp::options::msdp::detail::val:
                value_as_array().push_back({});
                break;

            default:
                value_as_array().back() += data;
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

    variable var_;
    std::vector<variable *> current_var_;

    state state_ = state::idle;
    std::function<void(variable const &)> cont_;
};

}  // namespace

// ==========================================================================
// DECODE
// ==========================================================================
void decode(
    telnetpp::bytes data, std::function<void(variable const &)> const &cont)
{
    parser parse{cont};

    for (auto const &by : data)
    {
        parse(by);
    }

    parse.finish();
}

}  // namespace telnetpp::options::msdp::detail
