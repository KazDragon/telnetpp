#pragma once

#include "telnetpp/core.hpp"
#include <boost/variant.hpp>
#include <initializer_list>
#include <string>
#include <vector>

namespace telnetpp { namespace options { namespace msdp {

static constexpr telnetpp::u8 option = 69;

static constexpr telnetpp::u8 var         = 1;
static constexpr telnetpp::u8 val         = 2;
static constexpr telnetpp::u8 table_open  = 3;
static constexpr telnetpp::u8 table_close = 4;
static constexpr telnetpp::u8 array_open  = 5;
static constexpr telnetpp::u8 array_close = 6;

struct variable;

using value_type = boost::variant<
    std::string,
    std::vector<std::string>,
    boost::recursive_wrapper<std::vector<variable>>
>;

//* =========================================================================
/// \brief A structure that represents a named MSDP variable whose value is
/// either a string, an array of strings, or an array of MSDP variables.
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
