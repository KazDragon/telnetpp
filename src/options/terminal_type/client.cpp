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
// REQUEST_TERMINAL_TYPE
// ==========================================================================
std::vector<telnetpp::token> client::request_terminal_type()
{
    return
    {
        telnetpp::element(
            telnetpp::subnegotiation(option(), { detail::send }))
    };
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
std::vector<telnetpp::token> client::handle_subnegotiation(
    u8stream const &content)
{
    auto begin = content.begin();
    auto end   = content.end();

    if (begin != end)
    {
        if (*begin++ == detail::is)
        {
            return on_terminal_type({begin, end});
        }
    }

    return {};
}

}}}
