#include "telnetpp/options/msdp/variable.hpp"
#include "telnetpp/detail/lambda_visitor.hpp"

namespace telnetpp { namespace options { namespace msdp {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
variable::variable()
{
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
variable::variable(
    telnetpp::bytes name, 
    string_value value)
  : name{name.begin(), name.end()},
    value{value}
{
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
variable::variable(
    telnetpp::bytes name,
    array_value array_values)
  : name{name.begin(), name.end()},
    value(array_values)
{
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
variable::variable(
    telnetpp::bytes name,
    table_value table_values)
  : name{name.begin(), name.end()},
    value(table_values)
{
}

/*
// ==========================================================================
// OPERATOR==
// ==========================================================================
bool operator<(variable const &lhs, variable const &rhs)
{
    auto const &lhs_value = lhs.value;
    
    if (lhs.name != rhs.name)
    {
        return lhs.name < rhs.name;
    }
    
    return detail::visit_lambdas<bool>(
        rhs.value,
        [&lhs_value](string_value const &str)
        {
            return false;
        },
        [&lhs_value](array_value const &arr)
        {
            return false;
        },
        [&lhs_value](object_value const &obj)
        {
            // TODO: unpack types here to give ordering.
            return true; 
        });
}

// ==========================================================================
// OPERATOR==
// ==========================================================================
bool operator==(variable const &lhs, variable const &rhs)
{
    auto const &lhs_value = lhs.value;

    return lhs.name == rhs.name
        && detail::visit_lambdas<bool>(
               rhs.value,
               [&lhs_value](auto const &inner_rhs)
               {
                   return boost::get<decltype(inner_rhs)>(lhs_value)
                       == inner_rhs;
               });
}

// ==========================================================================
// OPERATOR!=
// ==========================================================================
bool operator!=(variable const &lhs, variable const &rhs)
{
    return !(lhs == rhs);
}
*/

}}}
