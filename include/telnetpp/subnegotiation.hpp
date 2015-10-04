#ifndef TELNETPP_SUBNEGOTIATION_HPP_
#define TELNETPP_SUBNEGOTIATION_HPP_

#include "telnetpp/core.hpp"
#include <vector>

namespace telnetpp {

typedef std::vector<u8> subnegotiation_content;

//* =========================================================================
/// \brief A class that encapsulates a Telnet subnegotiation.
//* =========================================================================
class subnegotiation
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    subnegotiation(u8 option, subnegotiation_content content);
    
    //* =====================================================================
    /// \brief Returns the option for this subnegotiation.
    //* =====================================================================
    u8 option() const;

    //* =====================================================================
    /// \brief Returns the content for this subnegotiation.
    //* =====================================================================
    subnegotiation_content const &content() const;
    
private :
    u8 option_;
    subnegotiation_content content_;
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
