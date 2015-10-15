#ifndef TELNETPP_SUBNEGOTIATION_HPP_
#define TELNETPP_SUBNEGOTIATION_HPP_

#include "telnetpp/core.hpp"
#include <iosfwd>
#include <vector>

namespace telnetpp {

//* =========================================================================
/// \brief A class that encapsulates a Telnet subnegotiation.
//* =========================================================================
class subnegotiation
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
bool operator==(subnegotiation const &lhs, subnegotiation const &rhs);

//* =========================================================================
/// \brief Stream output for subnegotiations
//* =========================================================================
std::ostream &operator<<(std::ostream &out, subnegotiation const &cmd);

}

#endif
