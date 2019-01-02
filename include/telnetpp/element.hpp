#pragma once

#include "telnetpp/core.hpp"
#include "telnetpp/command.hpp"
#include "telnetpp/negotiation.hpp"
#include "telnetpp/subnegotiation.hpp"
#include <boost/variant.hpp>
#include <iosfwd>

namespace telnetpp {

//* =========================================================================
/// \class telnetpp::element
/// \brief A common type that can contain any Telnet operation, such as
/// a command, negotiation, or subnegotiation, or a string that represents
/// upper-layer non-Telnet data.
//* =========================================================================
using element = boost::variant<
    bytes,
    negotiation,
    subnegotiation,
    command
>;

//* =========================================================================
/// \brief Output operator for a telnetpp::element.
//* =========================================================================
std::ostream &operator<<(std::ostream &out, telnetpp::element const &elem);

}
