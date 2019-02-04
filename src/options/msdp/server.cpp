#include "telnetpp/options/msdp/server.hpp"
#include "telnetpp/options/msdp/detail/decoder.hpp"
#include "telnetpp/options/msdp/detail/protocol.hpp"

namespace telnetpp { namespace options { namespace msdp {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
server::server()
  : telnetpp::server_option(telnetpp::options::msdp::detail::option)
{
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
void server::handle_subnegotiation(
    telnetpp::bytes data,
    continuation const &cont)
{
    detail::decode(
        data, 
        [&](auto const &var)
        {
            on_receive(var, cont);
        });
}

}}}
