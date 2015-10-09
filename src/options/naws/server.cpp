#include "telnetpp/options/naws/server.hpp"
#include "telnetpp/options/naws.hpp"

namespace telnetpp { namespace options { namespace naws {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
server::server()
  : server_option(telnetpp::options::naws::option)
{
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
std::vector<telnetpp::token> server::handle_subnegotiation(
    std::vector<telnetpp::u8> const &content)
{
    if (content.size() == sizeof(telnetpp::u16) + sizeof(telnetpp::u16))
    {
        telnetpp::u16 width  = content[0] << 8 | content[1];
        telnetpp::u16 height = content[2] << 8 | content[3];
    
        return on_window_size_changed(width, height);
    }
    else
    {
        return {};
    }
}

}}}
