#pragma once

#include <boost/optional.hpp>
#include <string>

//* =========================================================================
/// \namespace telnetpp::options::new_environ
/// \brief An implementation of the Telnet Environment option
/// \par Overview
/// NEW-ENVIRON is used to send the values of variables from server to 
/// client.
/// \par
/// Variables are transmitted in pairs of 
/// telnetpp::options::new_environ::request and
/// telnetpp::options::new_environ::response, where the request is the type
/// of the variable (either a well known "var" type, the names of which are
/// limited -- see the specification), or a "uservar" type, which could have
/// any name.  The response may also contain a string if the variable
/// existed.
/// \par Usage
/// Create a server or client as appropriate.  Install it in a session,
/// activate as normal.
/// \par
/// A client may make a request of the server by calling request_variables().
/// It may also register for notifications about responses by subscribing to
/// the on_variable_changed signal.
/// \par
/// A server may set or delete variables and user variables, and will 
/// automatically handle requests sent from a client by using this data.
/// \see https://tools.ietf.org/html/rfc1572
//* =========================================================================
namespace telnetpp { namespace options { namespace new_environ { namespace detail {

static constexpr telnetpp::u8 option  = 39;

static constexpr telnetpp::u8 is      = 0;
static constexpr telnetpp::u8 send    = 1;
static constexpr telnetpp::u8 info    = 2;

static constexpr telnetpp::u8 var     = 0;
static constexpr telnetpp::u8 value   = 1;
static constexpr telnetpp::u8 esc     = 2;
static constexpr telnetpp::u8 uservar = 3;

}}}}
