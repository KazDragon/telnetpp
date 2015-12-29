#include "telnetpp/options/new_environ/client.hpp"
#include "telnetpp/options/new_environ.hpp"

namespace telnetpp { namespace options { namespace new_environ {

namespace {

// ==========================================================================
// PARSE_REQUESTS
// ==========================================================================
std::vector<request> parse_requests(u8stream const &data)
{
    std::vector<request> requests;
    request current_request;
    
    enum class state
    {
        send,
        var_or_uservar,
        name,
        name_esc
    };
    
    state current_state = state::send;
    
    for (auto &byte : data)
    {
        switch (current_state)
        {
            case state::send :
                if (byte != send)
                {
                    return {};
                }
                
                current_state = state::var_or_uservar;
                break;
                
            case state::var_or_uservar :
                if (byte == var || byte == uservar)
                {
                    current_request.type = byte;
                    current_state = state::name;
                }
                else
                {
                    return {};
                }
                break;
                
            case state::name :
                if (byte == var || byte == uservar)
                {
                    requests.push_back(current_request);
                    current_request.name  = "";
                    current_request.type  = byte;
                }
                else if (byte == esc)
                {
                    current_state = state::name_esc;
                }
                else
                {
                    current_request.name.push_back(byte);
                }
                break;
                
            case state::name_esc :
                current_request.name.push_back(byte);
                current_state = state::name;
                break;
                
            default :
                return {};
        }
    }
    
    if (!current_request.name.empty())
    {
        requests.push_back(current_request);
    }
    
    return requests;
}

// ==========================================================================
// VARIABLE_WAS_REQUESTED
// ==========================================================================
bool variable_was_requested(
    std::vector<request>                      const &requests,
    telnetpp::u8                                     variable_type,
    std::pair<std::string const, std::string> const &variable)
{
    if (requests.empty())
    {
        return true;
    }
    
    for (auto const &request : requests)
    {
        if (request.type == variable_type
         && request.name == variable.first)
        {
            return true;
        }
    }
    
    return false;
}

// ==========================================================================
// APPEND_STRING
// ==========================================================================
u8stream &append_string(
    u8stream          &stream,
    std::string const &text)
{
    for (auto ch : text)
    {
        switch (ch)
        {
            case var     : // Fall-through
            case value   : // Fall-through
            case esc     : // Fall-through
            case uservar : // Fall-through
                stream.push_back(esc);
                // Fall-through
            default :
                stream.push_back(ch);
                break;
        }
    }

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
    auto const &requests = parse_requests(content);
    
    u8stream result = { is };
    
    for (auto &variable : variables_)
    {
        if (variable_was_requested(requests, var, variable))
        {
            append_variable(result, var, variable);
        }
    }

    for (auto &variable : user_variables_)
    {
        if (variable_was_requested(requests, uservar, variable))
        {
            append_variable(result, uservar, variable);
        }
    }
    
    return { telnetpp::element{
        telnetpp::subnegotiation{option(), result}
    }};    
}


}}}
