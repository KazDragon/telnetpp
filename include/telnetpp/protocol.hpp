#ifndef TELNETPP_PROTOCOL_HPP_
#define TELNETPP_PROTOCOL_HPP_

#include "telnetpp/core.hpp"

namespace telnetpp {
 
// TELNET Commands
static constexpr u8 const eor  = 239; // End Of Record
static constexpr u8 const se   = 240; // Subnegotiation End
static constexpr u8 const nop  = 241; // No Operation
static constexpr u8 const dm   = 242; // Data Mark
static constexpr u8 const brk  = 243; // Break
static constexpr u8 const ip   = 244; // Interrupt Process
static constexpr u8 const ao   = 245; // Abort Output
static constexpr u8 const ayt  = 246; // Are You There?
static constexpr u8 const ec   = 247; // Erase Character
static constexpr u8 const el   = 248; // Erase Line
static constexpr u8 const ga   = 249; // Go Ahead
static constexpr u8 const sb   = 250; // Subnegotiation Begin
static constexpr u8 const will = 251;
static constexpr u8 const wont = 252;
static constexpr u8 const do_  = 253;
static constexpr u8 const dont = 254;
static constexpr u8 const iac  = 255; // Interpret As Command

}

#endif
