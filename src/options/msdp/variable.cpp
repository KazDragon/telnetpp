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
variable::variable(std::string const &name, value_type const &value)
  : name(name),
    value(value)
{
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
variable::variable(
    std::string const &name,
    std::initializer_list<std::string> const &il)
  : name(name),
    value(il)
{
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
variable::variable(
    std::string const &name,
    std::initializer_list<variable> const &il)
  : name(name),
    value(std::vector<variable>{il})
{
}

// ==========================================================================
// OPERATOR==
// ==========================================================================
bool operator==(variable const &lhs, variable const &rhs)
{
    return lhs.name == rhs.name
        && boost::apply_visitor(detail::make_lambda_visitor<bool>(
            [&lhs = lhs.value](auto const &inner_rhs)
            {
                return boost::get<decltype(inner_rhs)>(lhs) == inner_rhs;
            }), rhs.value);
}

// ==========================================================================
// OPERATOR!=
// ==========================================================================
bool operator!=(variable const &lhs, variable const &rhs)
{
    return !(lhs == rhs);
}

}}}
