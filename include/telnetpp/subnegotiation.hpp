﻿#pragma once

#include "telnetpp/core.hpp"

#include <iosfwd>
#include <utility>

namespace telnetpp {

//* =========================================================================
/// \brief A class that encapsulates a Telnet subnegotiation.
//* =========================================================================
class TELNETPP_EXPORT subnegotiation
{
   public:
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    constexpr subnegotiation(option_type option, bytes content) noexcept
      : option_(std::move(option)), content_(std::move(content))
    {
    }

    //* =====================================================================
    /// \brief Returns the option for this subnegotiation.
    //* =====================================================================
    [[nodiscard]] constexpr option_type option() const noexcept
    {
        return option_;
    }

    //* =====================================================================
    /// \brief Returns the content for this subnegotiation.
    //* =====================================================================
    [[nodiscard]] constexpr bytes content() const noexcept
    {
        return content_;
    }

   private:
    option_type option_;
    bytes content_;
};

//* =========================================================================
/// \brief Comparison function for subnegotiations
//* =========================================================================
gsl_constexpr20 inline bool operator==(
    subnegotiation const &lhs, subnegotiation const &rhs) noexcept
{
    return lhs.option() == rhs.option() &&
        telnetpp::bytes_equal(lhs.content(), rhs.content());
}

//* =========================================================================
/// \brief Stream output for subnegotiations
//* =========================================================================
TELNETPP_EXPORT
std::ostream &operator<<(std::ostream &out, subnegotiation const &sub);

}  // namespace telnetpp
