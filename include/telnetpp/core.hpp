#ifndef TELNETPP_TYPES_HPP_
#define TELNETPP_TYPES_HPP_

#include <boost/cstdint.hpp>
#include <vector>

namespace telnetpp {

using s8  = boost::int8_t;
using s16 = boost::int16_t;
using s32 = boost::int32_t;
using s64 = boost::int64_t;

using u8  = boost::uint8_t;
using u16 = boost::uint16_t;
using u32 = boost::uint32_t;
using u64 = boost::uint64_t;

using u8stream = std::vector<u8>;

}

#endif

