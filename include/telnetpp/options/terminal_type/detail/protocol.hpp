#pragma once

#include "telnetpp/core.hpp"

//* =========================================================================
/// \namespace telnetpp::options::terminal_type
/// \brief An implementation of the standard Telnet Terminal-Type option.
/// \par Overview
/// This option is used to exchange the name(s) of the current terminal
/// between server and client.
/// \par Server and Client
/// It may seem unintuitive at first, but it is the
/// telnetpp::options::terminal_type::client that requests the terminal type.
/// Normally, this client would be part of a server application.  However,
/// since it is the client that "owns" the data, it implements the server
/// portion of the option.  At least, that's the only way I can get my head
/// around it.
/// \see https://www.ietf.org/rfc/rfc1091.txt
//* =========================================================================
namespace telnetpp { namespace options { namespace terminal_type {
    namespace detail {

static constexpr option_type const option = 24;

using terminal_type_command_type = byte;
static constexpr terminal_type_command_type const is   = 0;
static constexpr terminal_type_command_type const send = 1;

}}}}
