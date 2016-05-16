#include "telnetpp/options/mccp/client.hpp"
#include "telnetpp/options/mccp/detail/protocol.hpp"

namespace telnetpp { namespace options { namespace mccp {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
client::client()
  : client_option(detail::option)
{
    on_state_changed.connect(
        [](client_option::state state)
            -> std::vector<telnetpp::token>
        {
            if (state == client_option::state::inactive)
            {
                return { boost::any(detail::end_decompression{}) };
            }
            else
            {
                return {};
            }
        });
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
std::vector<telnetpp::token> client::handle_subnegotiation(
    telnetpp::u8stream const &data)
{
    return { boost::any(detail::begin_decompression{}) };
}

}}}
