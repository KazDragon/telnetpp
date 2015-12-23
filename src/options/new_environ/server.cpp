#include "telnetpp/options/new_environ/server.hpp"
#include "telnetpp/options/new_environ.hpp"

namespace telnetpp { namespace options { namespace new_environ {

namespace {

enum class parse_state
{
    is_or_info,
    type,
    name,
    value,
};

// ==========================================================================
// PARSE_VALUE
// ==========================================================================
boost::optional<response> parse_value(
    parse_state &state, response &temp, u8 data)
{
    boost::optional<response> resp;
    
    if (data == telnetpp::options::new_environ::var
     || data == telnetpp::options::new_environ::uservar)
    {
        resp = temp;
        
        temp.type = data;
        temp.name = "";
        temp.value = boost::none;
        state = parse_state::name;
    }
    else
    {
        temp.value->push_back(data);
    }
    
    return resp;
}

// ==========================================================================
// PARSE_NAME
// ==========================================================================
boost::optional<response> parse_name(
    parse_state &state, response &temp, u8 data)
{
    boost::optional<response> resp;
    
    if (data == telnetpp::options::new_environ::var
     || data == telnetpp::options::new_environ::uservar)
    {
        resp = temp;

        temp.type = data;
        temp.name = "";
        temp.value = boost::none;
        state = parse_state::name;
    }
    else if (data == telnetpp::options::new_environ::value)
    {
        temp.value = "";
        state = parse_state::value;   
    }
    else
    {
        temp.name.push_back(char(data));
    }
    
    return resp;
}

// ==========================================================================
// PARSE_TYPE
// ==========================================================================
boost::optional<response> parse_type(
    parse_state &state, response &temp, u8 data)
{
    temp.type = data;
    state = parse_state::name;
    return {};
}

// ==========================================================================
// PARSE_IS_OR_INFO
// ==========================================================================
boost::optional<response> parse_is_or_info(
    parse_state &state, response &temp, u8 data)
{
    state = parse_state::type;
    return {};
}

// ==========================================================================
// PARSE_DATA
// ==========================================================================
boost::optional<response> parse_data(
    parse_state &state, response &temp, u8 data)
{
    boost::optional<response> resp;
    
    switch (state)
    {
        case parse_state::is_or_info :
            resp = parse_is_or_info(state, temp, data);
            break;

        case parse_state::type :
            resp = parse_type(state, temp, data);
            break;
            
        case parse_state::name :
            resp = parse_name(state, temp, data);
            break;
            
        case parse_state::value :
            resp = parse_value(state, temp, data);
            break;

        default :
            assert(!"Incorrect parse state.");
            break;
    }
    
    return resp;
}

// ==========================================================================
// PARSE_VARIABLE_CHANGES
// ==========================================================================
std::vector<response> parse_variable_changes(u8stream const &stream)
{
    std::vector<response> variable_changes;
    parse_state state = parse_state::is_or_info;
    
    response temp;
    boost::optional<response> resp;
    
    for (auto data : stream)
    {
        resp = parse_data(state, temp, data);
        
        if (resp)
        {
            variable_changes.push_back(*resp);
        }
    }
    
    if (!temp.name.empty())
    {
        variable_changes.push_back(temp);    
    }

    return variable_changes;    
}

}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
server::server()
  : server_option(telnetpp::options::new_environ::option)
{
    
}

// ==========================================================================
// REQUEST
// ==========================================================================
std::vector<token> server::request_variables(
    std::vector<request> const &requests)
{
    telnetpp::u8stream content;
    content.push_back(telnetpp::options::new_environ::send);
    
    for(auto &req : requests)
    {
        content.push_back(req.type);
        
        for (auto &ch : req.name)
        {
            switch (ch)
            {
                case telnetpp::options::new_environ::var :   // Fall-through
                case telnetpp::options::new_environ::value : // Fall-through
                case telnetpp::options::new_environ::esc :   // Fall-through
                case telnetpp::options::new_environ::uservar :
                    // Fall-through
                    content.push_back(telnetpp::options::new_environ::esc);
                default :
                    content.push_back(ch);
                    break;
            }
        }                
    }

    return {
        telnetpp::element { 
            telnetpp::subnegotiation(option(), content)
        }
    };
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
std::vector<token> server::handle_subnegotiation(u8stream const &stream)
{
    std::vector<token> responses;
    
    for (auto const &change : parse_variable_changes(stream))
    {
        auto const &response = on_variable_changed(change);
        responses.insert(responses.end(), response.begin(), response.end());
    }
    
    
    return responses;
}

}}}
