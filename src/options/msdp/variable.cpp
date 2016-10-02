#include "telnetpp/options/msdp/variable.hpp"

namespace telnetpp { namespace options { namespace msdp {

namespace {

// ==========================================================================
// \brief A class that determines if one variable's value equals another.
// ==========================================================================
class value_equals : public boost::static_visitor<bool>
{
public :
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    value_equals(value_type const &value)
      : value_(value)
    {
    }

    // ======================================================================
    // OPERATOR()
    // ======================================================================
    template <class Value>
    bool operator()(Value const &rhs) const
    {
        auto const *lhs = boost::get<Value>(&value_);

        return lhs == nullptr
             ? false
             : *lhs == rhs;
    }

private :
    value_type const &value_;
};

}

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
        && boost::apply_visitor(value_equals(lhs.value), rhs.value);
}

// ==========================================================================
// OPERATOR!=
// ==========================================================================
bool operator!=(variable const &lhs, variable const &rhs)
{
    return !(lhs == rhs);
}

}}}
