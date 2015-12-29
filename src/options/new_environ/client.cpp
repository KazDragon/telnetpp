#include "telnetpp/options/new_environ/client.hpp"
#include "telnetpp/options/new_environ.hpp"
#include <algorithm>

namespace telnetpp { namespace options { namespace new_environ {

namespace {

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
    std::copy(name.begin(), name.end(), back_inserter(stream));
    stream.push_back(value);
    std::copy(val.begin(), val.end(), back_inserter(stream));
    
    return stream;    
}

}    


// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
client::client()
  : telnetpp::client_option(telnetpp::options::new_environ::option)
{
}

// ==========================================================================
// SET_VARIABLE
// ==========================================================================
void client::set_variable(std::string const &name, std::string const &value)
{
    variables_[name] = value;
}

// ==========================================================================
// DELETE_VARIABLE
// ==========================================================================
void client::delete_variable(std::string const &name)
{
    variables_.erase(name);
}

// ==========================================================================
// SET_USER_VARIABLE
// ==========================================================================
void client::set_user_variable(
    std::string const &name, std::string const &value)
{
    user_variables_[name] = value;
}

// ==========================================================================
// DELETE_USER_VARIABLE
// ==========================================================================
void client::delete_user_variable(std::string const &name)
{
    user_variables_.erase(name);
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
std::vector<telnetpp::token> client::handle_subnegotiation(
    u8stream const &content)
{
    u8stream result = { is };
    
    for (auto &variable : variables_)
    {
        append_variable(result, var, variable);
    }

    for (auto &variable : user_variables_)
    {
        append_variable(result, uservar, variable);
    }
    
    return { telnetpp::element{
        telnetpp::subnegotiation{option(), result}
    }};    
}


}}}
