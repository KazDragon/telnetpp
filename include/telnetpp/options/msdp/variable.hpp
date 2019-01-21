#pragma once

#include "telnetpp/core.hpp"
#include <boost/variant.hpp>
#include <string>

namespace telnetpp { namespace options { namespace msdp {

struct variable;

using string_value = telnetpp::byte_storage;
using array_value  = std::basic_string<string_value>;
using table_value  = std::basic_string<variable>;

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
    variable(telnetpp::bytes name, array_value array_values);

    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    variable(telnetpp::bytes name, table_value table_values);

    telnetpp::byte_storage name;
    value_type value;
};

//* =========================================================================
/// \brief Less-than operator.
//* =========================================================================
//TELNETPP_EXPORT
//bool operator<(variable const &lhs, variable const &rhs);

//* =========================================================================
/// \brief Equality operator.
//* =========================================================================
//TELNETPP_EXPORT
//bool operator==(variable const &lhs, variable const &rhs);

//* =========================================================================
/// \brief Inequality operator.
//* =========================================================================
//TELNETPP_EXPORT
//bool operator!=(variable const &lhs, variable const &rhs);

}}}
