#pragma once

#include "telnetpp/detail/export.hpp"
#include <gsl/gsl-lite.hpp>
#include <string>
#include <cstdint>

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

// A stream of bytes in Telnet++ is exposed as a non-owning span.  It is
// expected that the data will live for no longer than any function in
// which it is found.  For that reason, these spans should never be stored.
// if this is necessary, it must be converted into a longer-term data
// structure, e.g. a std::vector, or std::basic_string<byte>.
using bytes = gsl::span<byte const>;

// Where necessary, bytes are stored in this type, which has the small
// string optimization, meaning that most cases will not cause an allocation.
using byte_storage = std::basic_string<byte>;

namespace literals {

// A simple function to convert from string literals to stored bytes.
inline byte_storage operator ""_tb(char const *text, size_t length)
{
    byte_storage result;
    result.reserve(length);

    for (auto ch : gsl::span<char const>{text, length})
    {
        result.push_back(static_cast<telnetpp::byte>(ch));
    }

    return result;
}

}}
