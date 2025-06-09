#include "telnetpp/command.hpp"

#include <format>
#include <iostream>

namespace telnetpp {

// ==========================================================================
// OPERATOR<<
// ==========================================================================
std::ostream &operator<<(std::ostream &out, command const &cmd)
{
    std::string_view cmd_type = [](command_type const type) {
        switch (type)
        {
            case telnetpp::nop:
                return "NOP";
            case telnetpp::dm:
                return "DM";
            case telnetpp::brk:
                return "BRK";
            case telnetpp::ip:
                return "IP";
            case telnetpp::ao:
                return "AO";
            case telnetpp::ayt:
                return "AYT";
            case telnetpp::ec:
                return "EC";
            case telnetpp::el:
                return "EL";
            case telnetpp::ga:
                return "GA";
            default:
                return "";
        }
    }(cmd.value());

    return cmd_type != ""
             ? (out << std::format("command[{}]", cmd_type))
             : (out << std::format("command[0x{:02X}]", cmd.value()));
}

}  // namespace telnetpp
