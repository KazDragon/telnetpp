#include "telnetpp/options/msdp/variable.hpp"
#include "telnetpp/detail/lambda_visitor.hpp"
#include <ostream>

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
  : name_{name.begin(), name.end()},
    value_{std::move(value)}
{
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
variable::variable(
    telnetpp::byte_storage name, 
    string_value value)
  : name_{std::move(name)},
    value_{std::move(value)}
{
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
variable::variable(
    telnetpp::bytes name,
    array_value array_values)
  : name_{name.begin(), name.end()},
    value_{array_values}
{
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
variable::variable(
    telnetpp::byte_storage name,
    array_value array_values)
  : name_{std::move(name)},
    value_{std::move(array_values)}
{
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
variable::variable(
    telnetpp::bytes name,
    table_value table_values)
  : name_{name.begin(), name.end()},
    value_{std::move(table_values)}
{
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
variable::variable(
    telnetpp::byte_storage name,
    table_value table_values)
  : name_{std::move(name)},
    value_{std::move(table_values)}
{
}

// ==========================================================================
// OPERATOR==
// ==========================================================================
bool operator==(variable const &lhs, variable const &rhs)
{
    auto const &lhs_value = lhs.value_;

    return lhs.name_ == rhs.name_
        && detail::visit_lambdas<bool>(
               rhs.value_,
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

// ==========================================================================
// OPERATOR<<
// ==========================================================================
std::ostream &operator<<(std::ostream &out, variable const &var)
{
    out << reinterpret_cast<char const *>(var.name_.c_str()) << "=";
    
    detail::visit_lambdas(
        var.value_,
        [&](string_value const &sv)
        {
            out << "\"" << reinterpret_cast<char const *>(sv.c_str()) << "\"";
        },
        [&](array_value const &av)
        {
            out << "[";
            
            for (auto const &item : av)
            {
                out << reinterpret_cast<char const *>(item.c_str()) << ",";
            }
            
            out << "]";
        },
        [&](table_value const &tv)
        {
            out << "{";
            
            for (auto const &inner_var :tv)
            {
                out << inner_var << ",";
            }

            out << "}";
        });
        
    return out;
}

}}}
