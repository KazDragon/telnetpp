#include "telnetpp/subnegotiation.hpp"
#include <iostream>
#include <utility>

namespace telnetpp {

// ==========================================================================
// OPERATOR<<
// ==========================================================================
std::ostream &operator<<(std::ostream& out, const subnegotiation& sub)
{
    out << "subnegotiation[0x"
        << std::hex << int(sub.option())
        << ", [";

    for (auto &&byte : sub.content())
    {
        out << "0x" << std::hex << int(byte) << ", ";
    }

    return out << "]]";
}

}
