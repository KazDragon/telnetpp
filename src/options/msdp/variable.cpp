#include "telnetpp/options/msdp/variable.hpp"
#include "telnetpp/detail/overloaded.hpp"
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
        && std::visit(detail::overloaded{
               [&lhs_value](string_value const &inner_rhs)
               {
                   return std::get<string_value>(lhs_value)
                       == inner_rhs;
               },
               [&lhs_value](array_value const &inner_rhs)
               {
                   return std::get<array_value>(lhs_value)
                       == inner_rhs;
               },
               [&lhs_value](table_value const &inner_rhs)
               {
                   return std::get<table_value>(lhs_value)
                       == inner_rhs;
               }},
               rhs.value_);
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
    
    std::visit(detail::overloaded{
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
        }},
        var.value_);
        
    return out;
}

}}}
