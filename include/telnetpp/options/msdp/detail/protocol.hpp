#pragma once

//* =========================================================================
/// \namespace telnetpp::options::msdp
/// \brief An implementation of the Mud Server Data Protocol
/// \par Overview
/// MSDP is used to send arbitrary data between server and client.
/// \par
/// There is no difference between the server and client protocols for MSDP:
/// both can send and receive arbitrary data packets.  However, it is usually
/// the case that the client sends a limited number of packets requesting or
/// subscribing to various standardised data sources.  This is, however,
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
namespace telnetpp { namespace options { namespace msdp {
    namespace detail {

static constexpr telnetpp::u8 option = 69;

static constexpr telnetpp::u8 var         = 1;
static constexpr telnetpp::u8 val         = 2;
static constexpr telnetpp::u8 table_open  = 3;
static constexpr telnetpp::u8 table_close = 4;
static constexpr telnetpp::u8 array_open  = 5;
static constexpr telnetpp::u8 array_close = 6;

}}}}
