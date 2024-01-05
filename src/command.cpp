#include "telnetpp/command.hpp"
#include <boost/io/ios_state.hpp>
#include <iomanip>
#include <iostream>

namespace telnetpp {

// ==========================================================================
// OPERATOR<<
// ==========================================================================
std::ostream &operator<<(std::ostream &out, command const &cmd)
{
  out << "command[";

  switch (cmd.value())
  {
    case telnetpp::nop:
      out << "NOP";
      break;
    case telnetpp::dm:
      out << "DM";
      break;
    case telnetpp::brk:
      out << "BRK";
      break;
    case telnetpp::ip:
      out << "IP";
      break;
    case telnetpp::ao:
      out << "AO";
      break;
    case telnetpp::ayt:
      out << "AYT";
      break;
    case telnetpp::ec:
      out << "EC";
      break;
    case telnetpp::el:
      out << "EL";
      break;
    case telnetpp::ga:
      out << "GA";
      break;
    default:
    {
      boost::io::ios_flags_saver ifs(out);
      out << "0x" << std::hex << std::setfill('0') << std::setw(2)
          << std::uppercase << static_cast<int>(cmd.value());
      break;
    }
  }

  return out << "]";
}

}  // namespace telnetpp
