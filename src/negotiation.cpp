#include "telnetpp/negotiation.hpp"
#include <boost/io/ios_state.hpp>
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
    }

    boost::io::ios_flags_saver ifs(out);
    return out << ", 0x" 
               << std::hex << std::setfill('0') << std::setw(2) << std::uppercase
               << int(neg.option_code()) << "]";
}

}
