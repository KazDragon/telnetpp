#pragma once

#include "telnetpp/core.hpp"

//* =========================================================================
/// \namespace telnetpp::options::echo
/// \brief An implementation of the standard Telnet Binary option.
/// \par Overview
/// The Binary option is used in a client/server relationship for agreeing
/// whether binary data (i.e. 8-bit data) may be transmitted.
/// \par Usage
/// Create an instance of an binary client or server, depending on which side
/// of the connection your application sits.  Install it into a session.
/// Activate as with any other option.
/// \see https://www.ietf.org/rfc/rfc856.txt
//* =========================================================================
namespace telnetpp::options::binary::detail {

inline constexpr option_type const option = 0;

}  // namespace telnetpp::options::binary::detail
