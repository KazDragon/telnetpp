#include "telnetpp/options/mccp/client.hpp"
#include "telnetpp/options/mccp/codec.hpp"
#include "telnetpp/options/mccp/detail/protocol.hpp"

namespace telnetpp { namespace options { namespace mccp {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
client::client(codec &cdc)
  : client_option(detail::option),
    codec_(cdc)
{
    on_state_changed.connect(
        [&](continuation const &cont)
        {
            if (!active())
            {
                codec_.finish(
                    [&](auto const &data, bool)
                    {
                        cont(data);
                    });
            }
        });
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
void client::handle_subnegotiation(
    telnetpp::bytes /*data*/,
    continuation const &/*cont*/)
{
    codec_.start();
}

}}}
