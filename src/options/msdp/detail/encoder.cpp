#include "telnetpp/options/msdp/detail/encoder.hpp"
#include "telnetpp/options/msdp/detail/protocol.hpp"
#include <algorithm>
#include <iterator>

using variable = telnetpp::options::msdp::variable;

namespace telnetpp { namespace options { namespace msdp { namespace detail {

namespace {

static void append_name(telnetpp::u8stream &stream, std::string const &name);
static void append_value(telnetpp::u8stream &stream, value_type const &value);
static telnetpp::u8stream &append_variable(
    telnetpp::u8stream &stream, variable const &variable);

class value_appender : public boost::static_visitor<>
{
public :
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    value_appender(telnetpp::u8stream &stream)
      : stream_(stream)
    {
    }

    // ======================================================================
    // OPERATOR()
    // ======================================================================
    void operator()(std::string const &string_value)
    {
        using std::begin;
        using std::end;

        stream_.push_back(telnetpp::options::msdp::detail::val);
        stream_.insert(end(stream_), begin(string_value), end(string_value));
    }

    // ======================================================================
    // OPERATOR()
    // ======================================================================
    void operator()(std::vector<std::string> const &array_value)
    {
        stream_.push_back(telnetpp::options::msdp::detail::val);
        stream_.push_back(telnetpp::options::msdp::detail::array_open);
        std::for_each(array_value.begin(), array_value.end(),
            [this](auto const &value){append_value(stream_, value);});
        stream_.push_back(telnetpp::options::msdp::detail::array_close);
    }

    // ======================================================================
    // OPERATOR()
    // ======================================================================
    void operator()(
        std::vector<variable> const &table)
    {
        stream_.push_back(telnetpp::options::msdp::detail::val);
        stream_.push_back(telnetpp::options::msdp::detail::table_open);
        std::for_each(table.begin(), table.end(),
            [this](auto const &variable)
            {
                append_variable(stream_, variable);
            });
        stream_.push_back(telnetpp::options::msdp::detail::table_close);
    }

private :
    telnetpp::u8stream &stream_;
};

// ==========================================================================
// APPEND_NAME
// ==========================================================================
static void append_name(telnetpp::u8stream &stream, std::string const &name)
{
    using std::begin;
    using std::end;

    stream.push_back(telnetpp::options::msdp::detail::var);
    stream.insert(end(stream), begin(name), end(name));
}

// ==========================================================================
// APPEND_VALUE
// ==========================================================================
static void append_value(telnetpp::u8stream &stream, value_type const &value)
{
    value_appender appender(stream);
    boost::apply_visitor(appender, value);
}

// ==========================================================================
// APPEND_VARIABLE
// ==========================================================================
static telnetpp::u8stream &append_variable(
    telnetpp::u8stream &stream,
    variable const &variable)
{
    append_name(stream, variable.name);
    append_value(stream, variable.value);

    return stream;
}

}

// ==========================================================================
// ENCODE
// ==========================================================================
telnetpp::u8stream encode(std::vector<variable> const &variables)
{
    return std::accumulate(
        variables.begin(),
        variables.end(),
        telnetpp::u8stream{},
        append_variable);
}

}}}}
