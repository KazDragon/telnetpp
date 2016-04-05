#include "telnetpp/options/new_environ/server.hpp"
#include "telnetpp/options/new_environ/detail/protocol.hpp"
#include "telnetpp/options/new_environ/detail/request_parser.hpp"
#include "telnetpp/options/new_environ/detail/stream.hpp"

namespace telnetpp { namespace options { namespace new_environ {

namespace {

// ==========================================================================
// VARIABLE_WAS_REQUESTED
// ==========================================================================
bool variable_was_requested(
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
u8stream &append_variable(
    u8stream                                        &stream,
    telnetpp::u8                                     type,
    std::pair<std::string const, std::string> const &variable)
{
    std::string const &name = variable.first;
    std::string const &val  = variable.second;

    stream.push_back(type);
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

    u8stream result = { detail::info };
    append_variable(result, detail::var, {name, value});

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

    u8stream result = { detail::info, detail::var };
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

    u8stream result = { detail::info };
    append_variable(result, detail::uservar, {name, value});

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

    u8stream result = { detail::info, detail::uservar };
    detail::append_escaped(result, name);

    return { telnetpp::element{
        telnetpp::subnegotiation(option(), result)
    }};
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
std::vector<telnetpp::token> server::handle_subnegotiation(
    u8stream const &content)
{
    auto const &requests = detail::parse_requests(content);

    u8stream result = { detail::is };

    for (auto &variable : variables_)
    {
        if (variable_was_requested(
            requests, variable_type::var, variable.first))
        {
            append_variable(result, detail::var, variable);
        }
    }

    for (auto &variable : user_variables_)
    {
        if (variable_was_requested(
            requests, variable_type::uservar, variable.first))
        {
            append_variable(result, detail::uservar, variable);
        }
    }

    return { telnetpp::element{
        telnetpp::subnegotiation{option(), result}
    }};
}


}}}
