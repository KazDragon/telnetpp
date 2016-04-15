#include "telnetpp/options/mccp/server.hpp"
#include "telnetpp/options/mccp/mccp.hpp"
#include "telnetpp/options/mccp/detail/protocol.hpp"

namespace telnetpp { namespace options { namespace mccp {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
server::server()
  : server_option(detail::option)
{
    on_state_changed.connect(
        [this](server_option::state) -> std::vector<telnetpp::token>
        {
            return { boost::any{block_token()} };
        });
}

}}}
