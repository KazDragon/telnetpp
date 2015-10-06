#ifndef TELNETPP_NEGOTIATION_HPP_
#define TELNETPP_NEGOTIATION_HPP_

#include "telnetpp/core.hpp"
#include <iosfwd>
#include <utility>

namespace telnetpp {
    
//* =========================================================================
/// \brief A class that encapsulates a Telnet negotiation.
//* =========================================================================
class negotiation
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    constexpr negotiation(u8 request, u8 option)
      : request_(std::move(request)),
        option_(std::move(option))
    {
    }

    //* =====================================================================
    /// \brief Returns the request (will, wont, do, dont) of this 
    /// negotiation.
    //* =====================================================================
    constexpr u8 request() const
    {
        return request_;
    }
    
    //* =====================================================================
    /// \brief Returns the option (e.g. echo, naws) of this negotiation
    //* =====================================================================
    constexpr u8 option() const
    {
        return option_;
    }
    
private :
    u8 request_;
    u8 option_;
};

//* =========================================================================
/// \brief Comparison function for negotiations
//* =========================================================================
constexpr bool operator==(negotiation const &lhs, negotiation const &rhs)
{
    return lhs.request() == rhs.request()
        && lhs.option() == rhs.option();
}

//* =========================================================================
/// \brief Stream output for negotiations
//* =========================================================================
std::ostream &operator<<(std::ostream &out, negotiation const &cmd);

}

#endif
