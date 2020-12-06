#include "telnetpp/subnegotiation.hpp"
#include <boost/io/ios_state.hpp>
#include <iomanip>
#include <iostream>
#include <utility>

namespace telnetpp {

// ==========================================================================
// OPERATOR<<
// ==========================================================================
std::ostream &operator<<(std::ostream& out, const subnegotiation& sub)
{
    boost::io::ios_all_saver ias(out);

    out << "subnegotiation[0x"
        << std::hex << std::setw(2) << std::setfill('0') << std::uppercase
        << int(sub.option())
        << ", [";

    bool first = true;

    for (auto &&byte : sub.content())
    {
        if (!std::exchange(first, false))
        {
            out << ", ";
        }

        out << "0x" << std::setw(2) << int(byte);
    }

    return out << "]]";
}

}
