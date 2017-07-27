#pragma once

#include "telnetpp/core.hpp"

//* =========================================================================
/// \namespace telnetpp::options::suppress_ga
/// \brief An implementation of the standard Telnet Suppress Go-Ahead option.
/// \par Overview
/// The Telnet specification suggests strongly that communications between
/// client and server follow a "half-duplex" protocol, in which the client
/// and server indicate that their "turn" is over by indicating that the
/// other end of the connection should "go ahead".  This option was designed
/// to enable the communications to engage in a "full-duplex" protocol, where
/// each party in the connection can transmit data at any time.
/// \par
/// While all modern servers and clients now begin with the assumption of a
/// full-duplex protocol, there is one extra feature in Suppress Go-Ahead
/// that remains useful: if a server has both this and the Echo option
/// active, then it is "commonly understood" that this should enable
/// character-at-a-time mode instead of the normal line-at-a-time mode.
/// \see https://tools.ietf.org/html/rfc858
/// \see telnetpp::options::echo
//* =========================================================================
namespace telnetpp { namespace options { namespace suppress_ga {
    namespace detail {

static constexpr option_type const option = 3;

}}}}
