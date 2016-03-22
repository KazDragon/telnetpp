#include "telnetpp/options/msdp.hpp"

namespace telnetpp { namespace options { namespace msdp {

namespace {

class value_equals : public boost::static_visitor<bool>
{
public :
    value_equals(value_type const &value)
      : value_(value)
    {
    }

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

variable::variable()
{
}

variable::variable(std::string const &name, value_type const &value)
  : name(name),
    value(value)
{
}

variable::variable(
    std::string const &name,
    std::initializer_list<std::string> const &il)
  : name(name),
    value(il)
{
}

variable::variable(
    std::string const &name,
    std::initializer_list<variable> const &il)
  : name(name),
    value(il)
{
}

bool operator==(variable const &lhs, variable const &rhs)
{
    return lhs.name == rhs.name
        && boost::apply_visitor(value_equals(lhs.value), rhs.value);
}

bool operator!=(variable const &lhs, variable const &rhs)
{
    return !(lhs == rhs);
}

}}}
