#include "telnetpp/options/msdp/variable.hpp"
#include "telnetpp/detail/overloaded.hpp"
#include <boost/algorithm/string/join.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <ostream>
#include <sstream>
#include <tuple>

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
    return std::tie(lhs.name_, lhs.value_) == std::tie(rhs.name_, rhs.value_);
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
    using boost::adaptors::transformed;

    static auto const to_string = [](auto const &str) {
        return std::string(reinterpret_cast<char const *>(str.c_str()));
    };

    out << to_string(var.name_) << "=";
    
    std::visit(detail::overloaded{
        [&](string_value const &sv)
        {
            out << "\"" << to_string(sv) << "\"";
        },
        [&](array_value const &av)
        {
            out << "[";
            out << boost::algorithm::join(av | transformed(to_string), ",");
            out << "]";
        },
        [&](table_value const &tv)
        {
            static auto const variable_to_string = [](auto const &tab_var) {
                std::stringstream stream;
                stream << tab_var;
                return stream.str();
            };

            out << "{";
            out << boost::algorithm::join(tv | transformed(variable_to_string), ",");
            out << "}";
        }},
        var.value_);
        
    return out;
}

}}}
