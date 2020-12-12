#include "telnetpp/options/mccp/server.hpp"
#include "telnetpp/options/mccp/codec.hpp"

namespace telnetpp { namespace options { namespace mccp {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
server::server(codec &cdc)
  : codec_(cdc),
    compression_active_(false)
{
    on_state_changed.connect(
        [this](auto &&cont)
        {
            if (compression_active_)
            {
                codec_.finish(
                    [&](auto const &data, bool)
                    {
                        cont(data);
                    });
                    
                compression_active_ = false;
            }
        });
}

// ==========================================================================
// BEGIN_COMPRESSION
// ==========================================================================
void server::start_compression(continuation const &cont)
{
    if (active())
    {
        element compression_start = subnegotiation{detail::option, {}};
        cont({compression_start});
        codec_.start();
        compression_active_ = true;
    }
}

// ==========================================================================
// END_COMPRESSION
// ==========================================================================
void server::finish_compression(continuation const &cont)
{
    if (active() && compression_active_)
    {
        codec_.finish(
            [&](auto const &data, bool)
            {
                cont(data);
            });
            
        compression_active_ = false;
    }
}

}}}
