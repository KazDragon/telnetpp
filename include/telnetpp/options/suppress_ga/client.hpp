#pragma once

#include "telnetpp/options/subnegotiationless_client.hpp"
#include "telnetpp/options/suppress_ga/detail/protocol.hpp"

namespace telnetpp { namespace options { namespace suppress_ga {

//* =========================================================================
/// \class telnetpp::options::suppress_ga::client
/// \extends telnetpp::client_option
/// \brief An implementation of the client side of the Telnet Suppress Go-
/// Ahead option.
/// \see https://tools.ietf.org/html/rfc858
//* =========================================================================
using client = telnetpp::options::subnegotiationless_client<
    telnetpp::options::suppress_ga::detail::option
>;

}}}
