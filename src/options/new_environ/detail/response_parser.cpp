#include "telnetpp/options/new_environ/detail/response_parser.hpp"

namespace telnetpp { namespace options { namespace new_environ {
    namespace detail {

namespace {
    
enum class parse_state
{
    is_or_info,
    type,
    name,
    name_esc,
    value,
    value_esc,
};

// ==========================================================================
// PARSE_VALUE_ESC
// ==========================================================================
boost::optional<response> parse_value_esc(
    parse_state &state, response &temp, u8 data)
{
    temp.value->push_back(data);
    state = parse_state::value;

    return {};
}

// ==========================================================================
// PARSE_VALUE
// ==========================================================================
boost::optional<response> parse_value(
    parse_state &state, response &temp, u8 data)
{
    boost::optional<response> resp;
    
    if (data == telnetpp::options::new_environ::esc)
    {
        state = parse_state::value_esc;
    }
    else if (data == telnetpp::options::new_environ::var
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
// PARSE_NAME_ESC
// ==========================================================================
boost::optional<response> parse_name_esc(
    parse_state &state, response &temp, u8 data)
{
    temp.name.push_back(char(data));
    state = parse_state::name;
    
    return {};
}

// ==========================================================================
// PARSE_NAME
// ==========================================================================
boost::optional<response> parse_name(
    parse_state &state, response &temp, u8 data)
{
    boost::optional<response> resp;
    
    if (data == telnetpp::options::new_environ::esc)
    {
        state = parse_state::name_esc;
    }
    else if (data == telnetpp::options::new_environ::var
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
            
        case parse_state::name_esc :
            resp = parse_name_esc(state, temp, data);
            break;
            
        case parse_state::value :
            resp = parse_value(state, temp, data);
            break;
            
        case parse_state::value_esc :
            resp = parse_value_esc(state, temp, data);
            break;

        default :
            return {};
            break;
    }
    
    return resp;
}

}

// ==========================================================================
// PARSE_RESPONSES
// ==========================================================================
std::vector<response> parse_responses(telnetpp::u8stream const &stream)
{
    std::vector<response> responses;
    parse_state state = parse_state::is_or_info;
    
    response temp;
    boost::optional<response> resp;
    
    for (auto data : stream)
    {
        resp = parse_data(state, temp, data);
        
        if (resp)
        {
            responses.push_back(*resp);
        }
    }
    
    if (!temp.name.empty())
    {
        responses.push_back(temp);    
    }

    return responses;    
}
        
}}}}