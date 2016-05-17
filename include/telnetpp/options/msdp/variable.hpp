#pragma once

#include "telnetpp/core.hpp"
#include <boost/variant.hpp>
#include <initializer_list>
#include <string>
#include <vector>

namespace telnetpp { namespace options { namespace msdp {

struct variable;

//* =========================================================================
/// \class telnetpp::options::msdp::value_type
/// \brief A variant that can either be a string, an array of string, or
/// an array of telnetpp::options::msdp::variable.
//* =========================================================================
using value_type = boost::variant<
    std::string,
    std::vector<std::string>,
    boost::recursive_wrapper<std::vector<variable>>
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
    variable(
        std::string const &name,
        value_type const &value);

    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    variable(
        std::string const &name,
        std::initializer_list<std::string> const &array_values);

    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    variable(
        std::string const &name,
        std::initializer_list<variable> const &table_values);

    std::string name;
    value_type  value;
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

}}}
