#include "telnetpp/options/terminal_type/client.hpp"
#include "telnetpp/options/terminal_type/detail/protocol.hpp"

namespace telnetpp { namespace options { namespace terminal_type {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
client::client()
  : client_option(telnetpp::options::terminal_type::detail::option)
{
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
void client::handle_subnegotiation(
    telnetpp::bytes data,
    continuation const &cont)
{
    if (!data.empty() && data[0] == detail::is)
    {
        on_terminal_type(data.subspan(1), cont);
    }
}

}}}
