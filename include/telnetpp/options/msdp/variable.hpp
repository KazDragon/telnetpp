#pragma once

#include "telnetpp/core.hpp"

#include <iosfwd>
#include <variant>
#include <vector>

namespace telnetpp::options::msdp {

struct variable;

using string_value = telnetpp::byte_storage;
using array_value = std::vector<string_value>;
using table_value = std::vector<variable>;

//* =========================================================================
/// \class telnetpp::options::msdp::value_type
/// \brief A variant that can either be a string, an array of string, or
/// an array of telnetpp::options::msdp::variable.
//* =========================================================================
using value_type = std::variant<string_value, array_value, table_value>;

//* =========================================================================
/// \brief A structure that represents a named value.
/// \see telnetpp::options::msdp::client
/// \see telnetpp::options::msdp::server
//* =========================================================================
struct TELNETPP_EXPORT variable
{
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    constexpr variable() = default;

    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    constexpr variable(telnetpp::byte_storage name, string_value value)
      : name_{std::move(name)}, value_{std::move(value)}
    {
    }

    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    constexpr variable(telnetpp::byte_storage name, array_value array_values)
      : name_{std::move(name)}, value_{std::move(array_values)}
    {
    }

    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    constexpr variable(telnetpp::byte_storage name, table_value table_values)
      : name_{std::move(name)}, value_{std::move(table_values)}
    {
    }

    //* =====================================================================
    /// \brief Equality operator
    //* =====================================================================
    TELNETPP_EXPORT
    constexpr friend bool operator==(
        variable const &lhs, variable const &rhs) noexcept = default;

    telnetpp::byte_storage name_;
    value_type value_;
};

//* =========================================================================
/// \brief Stream Output operator.
//* =========================================================================
TELNETPP_EXPORT
std::ostream &operator<<(std::ostream &out, variable const &var);

}  // namespace telnetpp::options::msdp
