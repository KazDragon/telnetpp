#include "telnetpp/options/new_environ/server.hpp"
#include "telnetpp/options/new_environ.hpp"

namespace telnetpp { namespace options { namespace new_environ {

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
std::vector<token> server::request_variables(std::vector<request> const &requests)
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
                case telnetpp::options::new_environ::var : // Fall-through
                case telnetpp::options::new_environ::value : // Fall-through
                case telnetpp::options::new_environ::esc : // Fall-through
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
    enum class parse_state
    {
        is_or_info,
        type,
        name,
        value,
    };
    
    parse_state state = parse_state::is_or_info;
    response resp;

    for (auto ch : stream) switch (state)
    {
        case parse_state::is_or_info :
            resp.name = "";
            resp.value = {};
            state = parse_state::type;
            break;
            
        case parse_state::type  :
            resp.type = ch;
            state = parse_state::name;
            break;
            
        case parse_state::name :
            if (ch == telnetpp::options::new_environ::var
             || ch == telnetpp::options::new_environ::uservar)
            {
                on_variable_changed(resp);
                resp.type = ch;
                resp.name = "";
                resp.value = {};
                state = parse_state::name;
            }
            else if (ch == telnetpp::options::new_environ::value)
            {
                resp.value = "";
                state = parse_state::value;   
            }
            else
            {
                resp.name.push_back(char(ch));
            }
            break;
            
        case parse_state::value :
            if (ch == telnetpp::options::new_environ::var
             || ch == telnetpp::options::new_environ::uservar)
            {
                on_variable_changed(resp);
                resp.type = ch;
                resp.name = "";
                resp.value = {};
                state = parse_state::name;
            }
            else
            {
                resp.value->push_back(ch);
            }
            break;

        default :
            break;
    }
    
    if (!resp.name.empty())
    {
        on_variable_changed(resp);
    }
    
    return {};    
}

}}}
