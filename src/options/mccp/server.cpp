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
        [this](server_option::state state)
        {
            std::vector<telnetpp::token> result;

            switch (state)
            {
                // In the case that the option is activating or deactivating,
                // this means that we have started the negotiation, and MUST
                // wait on the response to determine if further data is
                // compressed or not.  The block token is inserted into the
                // stream here to allow a lower level to start waiting.
                case server_option::state::deactivating :
                    // fall-through
                case server_option::state::activating :
                    result = { boost::any{block_token()} };
                    break;

                // In the case that the option is now active, it means that
                // either the remote client has agreed to compression, or has
                // requested it and we have agreed.  Either way, the
                // resume_compressed token is sent to the lower level to let it
                // know to unblock if it were blocked, and to send further data
                // in a compressed format.
                case server_option::state::active :
                    result = { boost::any{resume_compressed_token()} };
                    break;

                // In the case that the option is now inactive, it means that
                // either the remote client has rejected the option, or we have
                // decided to deactivate the option.  Either way, the
                // resume_uncompressed token is sent to the lower level to let
                // it know to unblock if it were blocked, and to send further
                // data in an uncompressed format.
                case server_option::state::inactive :
                    result = { boost::any{resume_uncompressed_token()} };
                    break;

                default :
                    assert(!"Invalid server option state");
                    break;
            }

            return result;
        });
}

}}}
