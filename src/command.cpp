#include "telnetpp/command.hpp"
#include "telnetpp/protocol.hpp"
#include <iostream>
#include <iomanip>

namespace telnetpp {
    
// ==========================================================================
// OPERATOR<<    
// ==========================================================================
std::ostream &operator<<(std::ostream &out, command const &cmd)
{
    out << "command[";
    
    switch (cmd.value())
    {
        case telnetpp::nop : out << "NOP"; break;
        case telnetpp::dm  : out << "DM"; break;
        case telnetpp::brk : out << "BRK"; break;
        case telnetpp::ip  : out << "IP"; break;
        case telnetpp::ao  : out << "AO"; break;
        case telnetpp::ayt : out << "AYT"; break;
        case telnetpp::ec  : out << "EC"; break;
        case telnetpp::el  : out << "EL"; break;
        case telnetpp::ga  : out << "GA"; break;
        default :
            out << "0x" << std::hex << u32(cmd.value());
            break;
    }
    
    return out << "]";
}

}
