#pragma once

#include "telnetpp/command.hpp"  // IWYU pragma: export
#include "telnetpp/core.hpp"
#include "telnetpp/negotiation.hpp"     // IWYU pragma: export
#include "telnetpp/subnegotiation.hpp"  // IWYU pragma: export

#include <iosfwd>
#include <variant>

namespace telnetpp {

//* =========================================================================
/// \class telnetpp::element
/// \brief A common type that can contain any Telnet operation, such as
/// a command, negotiation, or subnegotiation, or a string that represents
/// upper-layer non-Telnet data.
//* =========================================================================
using element = std::variant<bytes, negotiation, subnegotiation, command>;

//* =========================================================================
/// \brief Equality comparison operator for a telnetpp::element.
//* =========================================================================
gsl_constexpr20 inline bool operator==(
    element const &lhs, element const &rhs) noexcept
{
    auto visitor = [&rhs](auto const& _lhs) noexcept
    {
        using T = gsl::std20::remove_cvref_t<decltype(_lhs)>;
        if constexpr (std::is_same_v<T, bytes>)
        {
            return telnetpp::bytes_equal(_lhs, std::get<bytes>(rhs));
        }
        else
        {
            return _lhs == std::get<T>(rhs);
        }
    };
    if (lhs.index() != rhs.index()) return false;
    return std::visit(visitor, lhs);
}

//* =========================================================================
/// \brief A contiguous range of elements.
//* =========================================================================
using elements = gsl::span<element const>;

//* =========================================================================
/// \brief Output operator for a telnetpp::element.
//* =========================================================================
TELNETPP_EXPORT
std::ostream &operator<<(std::ostream &out, telnetpp::element const &elem);

}  // namespace telnetpp
