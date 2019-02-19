#include "telnetpp/options/mccp/server.hpp"
#include "telnetpp/options/mccp/codec.hpp"
#include "telnetpp/options/mccp/detail/protocol.hpp"

namespace telnetpp { namespace options { namespace mccp {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
server::server(codec &cdc)
  : server_option(detail::option),
    codec_(cdc),
    compression_requested_(false)
{
    on_state_changed.connect(
        [this](auto &&cont)
        {
            if (compression_requested_)
            {
                element compression_begin = subnegotiation{detail::option, {}};
                cont({compression_begin});
                codec_.start();
            }
        });
}

// ==========================================================================
// BEGIN_COMPRESSION
// ==========================================================================
void server::begin_compression(continuation const &cont)
{
    compression_requested_ = true;

    if (active())
    {
        element compression_begin = subnegotiation{detail::option, {}};
        cont({compression_begin});
        codec_.start();
    }
}

// ==========================================================================
// END_COMPRESSION
// ==========================================================================
void server::end_compression(continuation const &cont)
{
    compression_requested_ = false;

    if (active())
    {
        codec_.finish(
            [&](auto const &data, bool)
            {
                cont(data);
            });
    }
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
void server::handle_subnegotiation(
    telnetpp::bytes data,
    continuation const &cont)
{
}

}}}
