#pragma once

#include "telnetpp/command.hpp"
#include "telnetpp/negotiation.hpp"
#include "telnetpp/subnegotiation.hpp"
#include <boost/any.hpp>
#include <boost/variant.hpp>

namespace telnetpp {

//* =========================================================================
/// \class telnetpp::element
/// \brief A common type that can contain any Telnet operation, such as
/// a command, negotiation, or subnegotiation, or a string that represents
/// upper-layer non-Telnet data.  Finally, it may contain any other type,
/// which can be used to communicate between distant layers (e.g. an
/// encryption option handler and a lower layer encrypter)
//* =========================================================================
using element = boost::variant<
    gsl::span<byte>,
    negotiation,
    subnegotiation,
    command,
    boost::any
>;

}
