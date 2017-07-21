#pragma once

#include "telnetpp/detail/export.hpp"
#include <boost/cstdint.hpp>
#include <string>

namespace telnetpp {

using byte                        = std::uint8_t;
using option_type                 = std::uint8_t;
using command_type                = std::uint8_t;
using negotiation_type            = std::uint8_t;
using subnegotiation_content_type = byte;

// TELNET Commands
static constexpr command_type const se   = 240; // Subnegotiation End
static constexpr command_type const nop  = 241; // No Operation
static constexpr command_type const dm   = 242; // Data Mark
static constexpr command_type const brk  = 243; // Break
static constexpr command_type const ip   = 244; // Interrupt Process
static constexpr command_type const ao   = 245; // Abort Output
static constexpr command_type const ayt  = 246; // Are You There?
static constexpr command_type const ec   = 247; // Erase Character
static constexpr command_type const el   = 248; // Erase Line
static constexpr command_type const ga   = 249; // Go Ahead
static constexpr command_type const sb   = 250; // Subnegotiation Begin
static constexpr command_type const will = 251;
static constexpr command_type const wont = 252;
static constexpr command_type const do_  = 253;
static constexpr command_type const dont = 254;
static constexpr command_type const iac  = 255; // Interpret As Command

// Originally, Telnet++ used std::vector<u8> as its stream type.
// However, most Telnet packets tend to be small, and this means that
// it's philosophically possible to use the Small Buffer Optimization.
// std::vector<> is specified such that it's impossible to implement the
// SBO, whereas std::string has the SBO for most implementations.

// This may change in future to some other type, but it will always be
// something that models the basic standard Container concept.

//* =========================================================================
/// \class telnetpp::byte_stream
/// \brief A collection of bytes that models the Standard Library's
/// Container concept.  This is used for transporting arrays of bytes
/// across the library.
//* =========================================================================
using byte_stream = std::basic_string<byte>;

}
