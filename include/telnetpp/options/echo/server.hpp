#pragma once

#include "telnetpp/options/subnegotiationless_server.hpp"
#include "telnetpp/options/echo/detail/protocol.hpp"

namespace telnetpp { namespace options { namespace echo {

//* =========================================================================
/// \class telnetpp::options::echo::server
/// \extends telnetpp::server_option
/// \brief An implementation of the server side of Telnet ECHO option
/// \see https://tools.ietf.org/html/rfc857
//* =========================================================================
using server = telnetpp::options::subnegotiationless_server<
    telnetpp::options::echo::detail::option
>;

}}}
