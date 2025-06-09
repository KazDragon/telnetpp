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
constexpr bool operator==(element const &lhs, element const &rhs) noexcept
{
    auto const visitor = [&rhs](auto const &_lhs) noexcept {
        using T = std::decay_t<decltype(_lhs)>;
        if constexpr (std::is_same_v<T, bytes>)
        {
            return telnetpp::bytes_equal(_lhs, std::get<bytes>(rhs));
        }
        else
        {
            return _lhs == std::get<T>(rhs);
        }
    };

    return lhs.index() == rhs.index() ? std::visit(visitor, lhs) : false;
}

//* =========================================================================
/// \brief A contiguous range of elements.
//* =========================================================================
using elements = std::span<element const>;

//* =========================================================================
/// \brief Output operator for a telnetpp::element.
//* =========================================================================
TELNETPP_EXPORT
std::ostream &operator<<(std::ostream &out, telnetpp::element const &elem);

}  // namespace telnetpp
