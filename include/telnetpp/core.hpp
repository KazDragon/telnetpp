#pragma once

#include "telnetpp/detail/export.hpp"
#include <boost/cstdint.hpp>
#include <string>

namespace telnetpp {

using s8  = boost::int8_t;
using s16 = boost::int16_t;
using s32 = boost::int32_t;
using s64 = boost::int64_t;

//* =========================================================================
/// \typedef u8
/// \brief A typedef of an unsigned 8-bit integer that is used to represent
/// a byte of data.
//* =========================================================================
using u8  = boost::uint8_t;
using u16 = boost::uint16_t;
using u32 = boost::uint32_t;
using u64 = boost::uint64_t;

// Originally, Telnet++ used std::vector<u8> as its stream type.
// However, most Telnet packets tend to be small, and this means that
// it's philosophically possible to use the Small Buffer Optimization.
// std::vector<> is specified such that it's impossible to implement the
// SBO, whereas std::string has the SBO for most implementations.

// This may change in future to some other type, but it will always be
// something that models the basic standard Container concept.

//* =========================================================================
/// \class telnetpp::u8stream
/// \brief A collection of bytes that models the Standard Library's
/// Container concept.  This is used for transporting arrays of bytes
/// across the library.
//* =========================================================================
using u8stream = std::basic_string<u8>;

}
