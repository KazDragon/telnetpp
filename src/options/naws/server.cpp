#include "telnetpp/options/naws/server.hpp"

namespace telnetpp { namespace options { namespace naws {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
server::server()
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

}}}
