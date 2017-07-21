#include "telnetpp/negotiation.hpp"
#include "telnetpp/protocol.hpp"
#include <iostream>
#include <iomanip>

namespace telnetpp {

// ==========================================================================
// OPERATOR<<
// ==========================================================================
std::ostream &operator<<(std::ostream &out, negotiation const &neg)
{
    out << "negotiation[";

    switch (neg.request())
    {
        case telnetpp::will : out << "WILL"; break;
        case telnetpp::wont : out << "WONT"; break;
        case telnetpp::do_  : out << "DO"; break;
        case telnetpp::dont : out << "DONT"; break;
        default :
            out << "0x" << std::hex << int(neg.request());
            break;
    }

    return out << ",0x" << std::hex << int(neg.option()) << "]";
}

}
