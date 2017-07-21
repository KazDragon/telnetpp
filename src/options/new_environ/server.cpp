#include "telnetpp/options/new_environ/server.hpp"
#include "telnetpp/options/new_environ/detail/protocol.hpp"
#include "telnetpp/options/new_environ/detail/request_parser.hpp"
#include "telnetpp/options/new_environ/detail/stream.hpp"

namespace telnetpp { namespace options { namespace new_environ {

namespace {

// ==========================================================================
// TYPE_TO_BYTE
// ==========================================================================
static byte type_to_byte(
    telnetpp::options::new_environ::variable_type const &type)
{
    return type == variable_type::var
         ? telnetpp::options::new_environ::detail::var
         : telnetpp::options::new_environ::detail::uservar;
}

// ==========================================================================
// VARIABLE_WAS_REQUESTED
// ==========================================================================
static bool variable_was_requested(
    std::vector<request> const &requests,
    variable_type        const &type,
    std::string          const &name)
{
    if (requests.empty())
    {
        return true;
    }

    for (auto const &request : requests)
    {
        if (request.type == type && request.name == name)
        {
            return true;
        }
    }

    return false;
}

// ==========================================================================
// APPEND_VARIABLE
// ==========================================================================
static byte_stream &append_variable(
    byte_stream                                     &stream,
    variable_type                                    type,
    std::pair<std::string const, std::string> const &variable)
{
    std::string const &name = variable.first;
    std::string const &val  = variable.second;

    stream.push_back(type_to_byte(type));
    detail::append_escaped(stream, name);
    stream.push_back(detail::value);
    detail::append_escaped(stream, val);

    return stream;
}

}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
server::server()
  : telnetpp::server_option(telnetpp::options::new_environ::detail::option)
{
}

// ==========================================================================
// SET_VARIABLE
// ==========================================================================
std::vector<telnetpp::token> server::set_variable(
    std::string const &name, std::string const &value)
{
    variables_[name] = value;

    byte_stream result = { detail::info };
    append_variable(result, variable_type::var, {name, value});

    return { telnetpp::element{
        telnetpp::subnegotiation(option(), result)
    }};
}

// ==========================================================================
// DELETE_VARIABLE
// ==========================================================================
std::vector<telnetpp::token> server::delete_variable(std::string const &name)
{
    variables_.erase(name);

    byte_stream result = { detail::info, detail::var };
    detail::append_escaped(result, name);

    return { telnetpp::element{
        telnetpp::subnegotiation(option(), result)
    }};
}

// ==========================================================================
// SET_USER_VARIABLE
// ==========================================================================
std::vector<telnetpp::token> server::set_user_variable(
    std::string const &name, std::string const &value)
{
    user_variables_[name] = value;

    byte_stream result = { detail::info };
    append_variable(result, variable_type::uservar, {name, value});

    return { telnetpp::element{
        telnetpp::subnegotiation(option(), result)
    }};
}

// ==========================================================================
// DELETE_USER_VARIABLE
// ==========================================================================
std::vector<telnetpp::token> server::delete_user_variable(
    std::string const &name)
{
    user_variables_.erase(name);

    byte_stream result = { detail::info, detail::uservar };
    detail::append_escaped(result, name);

    return { telnetpp::element{
        telnetpp::subnegotiation(option(), result)
    }};
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
std::vector<telnetpp::token> server::handle_subnegotiation(
    byte_stream const &content)
{
    auto const &requests = detail::parse_requests(content);

    byte_stream result = { detail::is };

    for (auto &variable : variables_)
    {
        if (variable_was_requested(
            requests, variable_type::var, variable.first))
        {
            append_variable(result, variable_type::var, variable);
        }
    }

    for (auto &variable : user_variables_)
    {
        if (variable_was_requested(
            requests, variable_type::uservar, variable.first))
        {
            append_variable(result, variable_type::uservar, variable);
        }
    }

    return { telnetpp::element{
        telnetpp::subnegotiation{option(), result}
    }};
}


}}}
