#pragma once

#include "telnetpp/core.hpp"
#include <boost/container/small_vector.hpp>
#include <boost/variant.hpp>
#include <iosfwd>
#include <vector>

namespace telnetpp { namespace options { namespace msdp {

struct variable;

using string_value = telnetpp::byte_storage;
using array_value  = boost::container::small_vector<string_value, 4>;
using table_value  = std::vector<variable>;

//* =========================================================================
/// \class telnetpp::options::msdp::value_type
/// \brief A variant that can either be a string, an array of string, or
/// an array of telnetpp::options::msdp::variable.
//* =========================================================================
using value_type = boost::variant<
    string_value,
    array_value,
    boost::recursive_wrapper<table_value>
>;

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
    variable();

    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    variable(telnetpp::bytes name, string_value value);

    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    variable(telnetpp::byte_storage name, string_value value);

    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    variable(telnetpp::bytes name, array_value array_values);

    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    variable(telnetpp::byte_storage name, array_value array_values);

    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    variable(telnetpp::bytes name, table_value table_values);

    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    variable(telnetpp::byte_storage name, table_value table_values);
    
    telnetpp::byte_storage name_;
    value_type value_;
};

//* =========================================================================
/// \brief Equality operator.
//* =========================================================================
TELNETPP_EXPORT
bool operator==(variable const &lhs, variable const &rhs);

//* =========================================================================
/// \brief Inequality operator.
//* =========================================================================
TELNETPP_EXPORT
bool operator!=(variable const &lhs, variable const &rhs);

//* =========================================================================
/// \brief Stream Output operator.
//* =========================================================================
TELNETPP_EXPORT
std::ostream &operator<<(std::ostream &out, variable const &var);

}}}
