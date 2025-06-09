#pragma once

#include "telnetpp/detail/export.hpp"  // IWYU pragma: export

#include <algorithm>
#include <span>
#include <string>
#include <cstdint>

namespace telnetpp {

using byte = std::uint8_t;
using option_type = std::uint8_t;
using command_type = std::uint8_t;
using negotiation_type = std::uint8_t;
using subnegotiation_content_type = byte;

// TELNET Commands
inline constexpr command_type const se = 240;   // Subnegotiation End
inline constexpr command_type const nop = 241;  // No Operation
inline constexpr command_type const dm = 242;   // Data Mark
inline constexpr command_type const brk = 243;  // Break
inline constexpr command_type const ip = 244;   // Interrupt Process
inline constexpr command_type const ao = 245;   // Abort Output
inline constexpr command_type const ayt = 246;  // Are You There?
inline constexpr command_type const ec = 247;   // Erase Character
inline constexpr command_type const el = 248;   // Erase Line
inline constexpr command_type const ga = 249;   // Go Ahead
inline constexpr command_type const sb = 250;   // Subnegotiation Begin
inline constexpr command_type const will = 251;
inline constexpr command_type const wont = 252;
inline constexpr command_type const do_ = 253;  // NOLINT
inline constexpr command_type const dont = 254;
inline constexpr command_type const iac = 255;  // Interpret As Command

// A stream of bytes in Telnet++ is exposed as a non-owning span.  It is
// expected that the data will live for no longer than any function in
// which it is found.  For that reason, these spans should never be stored.
// if this is necessary, it must be converted into a longer-term data
// structure, e.g. a std::vector, or std::basic_string<byte>.
using bytes = std::span<byte const>;

// Where necessary, bytes are stored in this type, which has the small
// string optimization, meaning that most cases will not cause an allocation.
using byte_storage = std::basic_string<byte>;

// Comparison function for bytes.
// std::span<> does not define comparison operators by default because the
// semantics are unclear (deep or shallow?).  This named comparison function
// is provided because we cannot define a useful operator== because ADL
// would not find it in our namespace.
constexpr inline auto bytes_equal = [](bytes const &lhs,
                                       bytes const &rhs) noexcept {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
};

namespace literals {

// A simple function to convert from string literals to stored bytes.
inline byte_storage operator""_tb(char const *text, size_t length)
{
    byte_storage result;
    result.reserve(length);

    for (auto ch : std::span<char const>{text, length})
    {
        result.push_back(static_cast<telnetpp::byte>(ch));
    }

    return result;
}

}  // namespace literals
}  // namespace telnetpp
