#include "telnetpp/options/terminal_type/client.hpp"
#include "telnetpp/options/terminal_type.hpp"

namespace telnetpp { namespace options { namespace terminal_type {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
client::client()
  : client_option(telnetpp::options::terminal_type::option)
{
}

// ==========================================================================
// REQUEST_TERMINAL_TYPE
// ==========================================================================
std::vector<telnetpp::token> client::request_terminal_type()
{
    return 
    { 
        telnetpp::subnegotiation(
            option(), { telnetpp::options::terminal_type::send })
    };
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
std::vector<telnetpp::token> client::handle_subnegotiation(
    std::vector<telnetpp::u8> const &content)
{
    auto begin = content.begin();
    auto end   = content.end();
    
    if (begin != end)
    {
        if (*begin++ == telnetpp::options::terminal_type::is)
        {
            return on_terminal_type({begin, end});
        }
    }
    
    return {};
}

}}}
