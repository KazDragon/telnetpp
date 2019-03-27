#include "telnetpp/options/naws/server.hpp"
#include "telnetpp/options/naws/detail/protocol.hpp"

namespace telnetpp { namespace options { namespace naws {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
server::server()
  : server_option(telnetpp::options::naws::detail::option)
{
    on_state_changed.connect(
        [this](auto &&cont)
        {
            if (this->active())
            {
                this->report_window_size(cont);
            }
        });
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
void server::handle_subnegotiation(
    telnetpp::bytes content,
    continuation const &cont)
{
}

}}}
