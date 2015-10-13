#include "telnetpp/options/naws/client.hpp"
#include "telnetpp/options/naws.hpp"

namespace telnetpp { namespace options { namespace naws {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
client::client()
  : client_option(telnetpp::options::naws::option)
{
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
std::vector<telnetpp::token> client::handle_subnegotiation(
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
