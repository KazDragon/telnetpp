#pragma once

#include "telnetpp/core.hpp"

//* =========================================================================
/// \namespace telnetpp::options::echo
/// \brief An implementation of the standard Telnet Echo option.
/// \par Overview
/// The Echo option is used in a client/server relationship for agreeing
/// whether the server should echo the client or not.  In many cases, such
/// as communicating with a remote shell, the user is better served by
/// having his client print out the results of his or her keypresses
/// immediately.  This is the default, with Echo disabled.  The alternative,
/// which is achieved by activating the Echo option, is for the server to
/// print out the result of the keypresses.  While this increases the time
/// it takes to see the result of an operation, it also allows the server
/// to show application-specific behaviour instead of just printing the value
/// of the pressed key.
/// \par Usage
/// Create an instance of an echo client or server, depending on which side
/// of the connection your application sits.  Install it into a session.
/// Activate as with any other option.
/// \see https://www.ietf.org/rfc/rfc857.txt
//* =========================================================================
namespace telnetpp { namespace options { namespace echo { namespace detail {

static constexpr telnetpp::u8 option = 1;

}}}}
