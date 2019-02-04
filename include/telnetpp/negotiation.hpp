#pragma once

#include "telnetpp/core.hpp"
#include "telnetpp/detail/hash.hpp"
#include <iosfwd>
#include <utility>

namespace telnetpp {

//* =========================================================================
/// \brief A class that encapsulates a Telnet negotiation.
//* =========================================================================
class TELNETPP_EXPORT negotiation
{
public:
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    constexpr negotiation(negotiation_type request, option_type option) noexcept
      : request_(request),
        option_(option)
    {
    }

    //* =====================================================================
    /// \brief Returns the request (will, wont, do, dont) of this
    /// negotiation.
    //* =====================================================================
    constexpr negotiation_type request() const noexcept
    {
        return request_;
    }

    //* =====================================================================
    /// \brief Returns the option (e.g. echo, naws) of this negotiation
    //* =====================================================================
    constexpr option_type option() const noexcept
    {
        return option_;
    }

    //* =====================================================================
    /// \brief Combine a hash of the object
    //* =====================================================================
    constexpr void hash_combine(std::size_t &seed) const
    {
        telnetpp::detail::hash_combine(seed, request_, option_);
    }

private:
    negotiation_type request_;
    option_type option_;
};

//* =========================================================================
/// \brief Comparison function for negotiations
//* =========================================================================
constexpr bool operator==(negotiation const &lhs, negotiation const &rhs) noexcept
{
    return lhs.request() == rhs.request()
        && lhs.option() == rhs.option();
}

//* =========================================================================
/// \brief Ordering function for negotiations
//* =========================================================================
constexpr bool operator<(negotiation const &lhs, negotiation const &rhs) noexcept
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

// This is necessary because the lookup for the recipient for a negotiation
// in a session relies on both the request and the option type (will/wont
// go to a different place than do/dont, for example).  Therefore, the entire
// negotiation is the key in the lookup and needs to be hashable to go to the
// unordered map.
TELNETPP_MAKE_INTRUSIVE_HASH(telnetpp::negotiation);