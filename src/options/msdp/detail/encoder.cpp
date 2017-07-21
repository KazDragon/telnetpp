#include "telnetpp/options/msdp/detail/encoder.hpp"
#include "telnetpp/options/msdp/detail/protocol.hpp"
#include "telnetpp/detail/lambda_visitor.hpp"
#include <algorithm>
#include <iterator>
#include <numeric>

using variable = telnetpp::options::msdp::variable;

namespace telnetpp { namespace options { namespace msdp { namespace detail {

namespace {

static void append_name(
    telnetpp::byte_stream &stream, std::string const &name);

static void append_value(
    telnetpp::byte_stream &stream, value_type const &value);

static telnetpp::byte_stream &append_variable(
    telnetpp::byte_stream &stream, variable const &variable);

// ==========================================================================
// APPEND_NAME
// ==========================================================================
static void append_name(telnetpp::byte_stream &stream, std::string const &name)
{
    using std::begin;
    using std::end;

    stream.push_back(telnetpp::options::msdp::detail::var);
    stream.insert(end(stream), begin(name), end(name));
}

// ==========================================================================
// APPEND_STRING_VALUE
// ==========================================================================
static void append_string_value(
    telnetpp::byte_stream &stream,
    std::string const &string_value)
{
    using std::begin;
    using std::end;

    stream.push_back(telnetpp::options::msdp::detail::val);
    stream.insert(end(stream), begin(string_value), end(string_value));
}

// ==========================================================================
// APPEND_ARRAY_VALUE
// ==========================================================================
static void append_array_value(
    telnetpp::byte_stream &stream,
    std::vector<std::string> const &array_value)
{
    stream.push_back(telnetpp::options::msdp::detail::val);
    stream.push_back(telnetpp::options::msdp::detail::array_open);
    std::for_each(array_value.begin(), array_value.end(),
        [&stream](auto const &value){append_value(stream, value);});
    stream.push_back(telnetpp::options::msdp::detail::array_close);
}

// ==========================================================================
// APPEND_TABLE_VALUE
// ==========================================================================
static void append_table_value(
    telnetpp::byte_stream& stream,
    std::vector<variable> const &table)
{
    stream.push_back(telnetpp::options::msdp::detail::val);
    stream.push_back(telnetpp::options::msdp::detail::table_open);
    std::for_each(table.begin(), table.end(),
        [&stream](auto const &variable)
        {
            append_variable(stream, variable);
        });
    stream.push_back(telnetpp::options::msdp::detail::table_close);
}

// ==========================================================================
// APPEND_VALUE
// ==========================================================================
static void append_value(
    telnetpp::byte_stream &stream, value_type const &value)
{
    telnetpp::detail::visit_lambdas(
        value,
        [&stream](std::string const &string_value)
        {
            append_string_value(stream, string_value);
        },
        [&stream](std::vector<std::string> const &array_value)
        {
            append_array_value(stream, array_value);
        },
        [&stream](std::vector<variable> const &table_value)
        {
            append_table_value(stream, table_value);
        });
}

// ==========================================================================
// APPEND_VARIABLE
// ==========================================================================
static telnetpp::byte_stream &append_variable(
    telnetpp::byte_stream &stream,
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
telnetpp::byte_stream encode(std::vector<variable> const &variables)
{
    return std::accumulate(
        variables.begin(),
        variables.end(),
        telnetpp::byte_stream{},
        append_variable);
}

}}}}
