#include "telnetpp/subnegotiation.hpp"

#include <format>
#include <iostream>

namespace telnetpp {

// ==========================================================================
// OPERATOR<<
// ==========================================================================
std::ostream &operator<<(std::ostream &out, subnegotiation const &sub)
{
    out << std::format("subnegotiation[0x{:02X}, [", sub.option());

    bool first = true;

    for (auto &&by : sub.content())
    {
        if (!std::exchange(first, false))
        {
            out << ", ";
        }

        out << std::format("0x{:02X}", static_cast<int>(by));
    }

    return out << "]]";
}

}  // namespace telnetpp
