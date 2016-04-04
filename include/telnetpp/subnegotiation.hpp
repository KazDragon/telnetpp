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
    subnegotiation(u8 option, u8stream content);

    //* =====================================================================
    /// \brief Returns the option for this subnegotiation.
    //* =====================================================================
    u8 option() const;

    //* =====================================================================
    /// \brief Returns the content for this subnegotiation.
    //* =====================================================================
    u8stream const &content() const;

private :
    u8 option_;
    u8stream content_;
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
