#include "telnetpp/options/mccp/client.hpp"
#include "telnetpp/options/mccp/mccp.hpp"
#include "telnetpp/options/mccp/detail/protocol.hpp"

namespace telnetpp { namespace options { namespace mccp {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
client::client()
  : client_option(detail::option)
{
    on_state_changed.connect(
        []
        {
           return std::vector<telnetpp::token>{
               boost::any(end_decompression{})
           };
        });
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
std::vector<telnetpp::token> client::handle_subnegotiation(
    telnetpp::u8stream const &data)
{
    return { boost::any(begin_decompression{}) };
}

}}}
