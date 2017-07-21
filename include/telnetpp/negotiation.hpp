#pragma once

#include "telnetpp/core.hpp"
#include <iosfwd>
#include <utility>

namespace telnetpp {

//* =========================================================================
/// \brief A class that encapsulates a Telnet negotiation.
//* =========================================================================
class TELNETPP_EXPORT negotiation
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    constexpr negotiation(negotiation_type request, option_type option)
      : request_(request),
        option_(option)
    {
    }

    //* =====================================================================
    /// \brief Returns the request (will, wont, do, dont) of this
    /// negotiation.
    //* =====================================================================
    constexpr negotiation_type request() const
    {
        return request_;
    }

    //* =====================================================================
    /// \brief Returns the option (e.g. echo, naws) of this negotiation
    //* =====================================================================
    constexpr option_type option() const
    {
        return option_;
    }

private :
    negotiation_type request_;
    option_type option_;
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
/// \brief Ordering function for negotiations
//* =========================================================================
constexpr bool operator<(negotiation const &lhs, negotiation const &rhs)
{
    return lhs.request() < rhs.request()
        || (!(rhs.request() < lhs.request()) && lhs.option() < rhs.option());
}

//* =========================================================================
/// \brief Stream output for negotiations
//* =========================================================================
TELNETPP_EXPORT
std::ostream &operator<<(std::ostream &out, negotiation const &cmd);

}
