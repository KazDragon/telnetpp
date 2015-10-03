#ifndef TELNETPP_TOKEN_HPP_
#define TELNETPP_TOKEN_HPP_

#include "telnetpp/command.hpp"
#include "telnetpp/negotiation.hpp"
#include "telnetpp/subnegotiation.hpp"
#include <boost/variant.hpp>
#include <string>

namespace telnetpp {
    
//* =========================================================================
/// \brief A common token type for all Telnet operations.
//* =========================================================================
typedef boost::variant<
    std::string,
    negotiation,
    subnegotiation,
    command
> token;

}

#endif
