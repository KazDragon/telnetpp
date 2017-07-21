#pragma once

#include "telnetpp/core.hpp"
#include <iosfwd>
#include <vector>

namespace telnetpp {

//* =========================================================================
/// \brief A class that encapsulates a Telnet subnegotiation.
//* =========================================================================
class TELNETPP_EXPORT subnegotiation
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    subnegotiation(option_type option, byte_stream content);

    //* =====================================================================
    /// \brief Returns the option for this subnegotiation.
    //* =====================================================================
    option_type option() const;

    //* =====================================================================
    /// \brief Returns the content for this subnegotiation.
    //* =====================================================================
    byte_stream const &content() const;

private :
    option_type option_;
    byte_stream content_;
};

//* =========================================================================
/// \brief Comparison function for subnegotiations
//* =========================================================================
TELNETPP_EXPORT
bool operator==(subnegotiation const &lhs, subnegotiation const &rhs);

//* =========================================================================
/// \brief Stream output for subnegotiations
//* =========================================================================
TELNETPP_EXPORT
std::ostream &operator<<(std::ostream &out, subnegotiation const &cmd);

}
