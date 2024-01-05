#pragma once

#include "telnetpp/core.hpp"

//* =========================================================================
/// \namespace telnetpp::options::msdp
/// \brief An implementation of the Mud Server Data Protocol
/// \par Overview
/// MSDP is used to send arbitrary data between server and client.
/// \par
/// There is no difference between the server and client protocols for MSDP:
/// both can send and receive arbitrary data packets.  However, it is usually
/// the case that the client sends a limited number of packets requesting or
/// subscribing to various standardized data sources.  This is, however,
/// entirely application-specific.  See the specification page for more
/// details.
/// \par Usage
/// Create a server or client as appropriate.  Install it in a session,
/// activate as normal.
/// \par
/// Data is transmitted using a telnetpp::options::msdp::variable.  These
/// can be sent using your option's send() function, or you can register for
/// changes using the on_receive signal.
/// \see http://tintin.sourceforge.net/msdp/
//* =========================================================================
namespace telnetpp::options::msdp::detail {

static constexpr option_type const option = 69;

using msdp_command_type = byte;
static constexpr msdp_command_type const var = 1;
static constexpr msdp_command_type const val = 2;
static constexpr msdp_command_type const table_open = 3;
static constexpr msdp_command_type const table_close = 4;
static constexpr msdp_command_type const array_open = 5;
static constexpr msdp_command_type const array_close = 6;

}  // namespace telnetpp::options::msdp::detail
