#include "telnetpp/options/mccp/client.hpp"
#include "telnetpp/options/mccp/decompressor.hpp"
#include "telnetpp/options/mccp/detail/protocol.hpp"

namespace telnetpp { namespace options { namespace mccp {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
client::client(decompressor &dec)
  : client_option(detail::option),
    decompressor_(dec)
{
    on_state_changed.connect(
        [&](continuation const &cont)
        {
            if (!active())
            {
                decompressor_.end_decompression();
            }
        });
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
void client::handle_subnegotiation(
    telnetpp::bytes data,
    continuation const &cont)
{
    decompressor_.start_decompression();
}

}}}
