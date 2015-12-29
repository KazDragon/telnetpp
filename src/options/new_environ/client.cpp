#include "telnetpp/options/new_environ/client.hpp"
#include "telnetpp/options/new_environ.hpp"
#include <algorithm>

namespace telnetpp { namespace options { namespace new_environ {

namespace {

// ==========================================================================
// APPEND_STRING
// ==========================================================================
u8stream &append_string(
    u8stream          &stream,
    std::string const &text)
{
    std::copy(text.begin(), text.end(), back_inserter(stream));
    return stream;
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
    append_string(stream, name);
    stream.push_back(value);
    append_string(stream, val);
    
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
std::vector<telnetpp::token> client::set_variable(
    std::string const &name, std::string const &value)
{
    variables_[name] = value;

    u8stream result = { info };
    append_variable(result, var, {name, value});
    
    return { telnetpp::element{ 
        telnetpp::subnegotiation(option(), result) 
    }};
}

// ==========================================================================
// DELETE_VARIABLE
// ==========================================================================
std::vector<telnetpp::token> client::delete_variable(std::string const &name)
{
    variables_.erase(name);
    
    u8stream result = { info, var };
    append_string(result, name);
    
    return { telnetpp::element{
        telnetpp::subnegotiation(option(), result)
    }};
}

// ==========================================================================
// SET_USER_VARIABLE
// ==========================================================================
std::vector<telnetpp::token> client::set_user_variable(
    std::string const &name, std::string const &value)
{
    user_variables_[name] = value;
    
    u8stream result = { info };
    append_variable(result, uservar, {name, value});
    
    return { telnetpp::element{
        telnetpp::subnegotiation(option(), result)
    }};
}

// ==========================================================================
// DELETE_USER_VARIABLE
// ==========================================================================
std::vector<telnetpp::token> client::delete_user_variable(
    std::string const &name)
{
    user_variables_.erase(name);
    
    u8stream result = { info, uservar };
    append_string(result, name);
    
    return { telnetpp::element{
        telnetpp::subnegotiation(option(), result)
    }};
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
