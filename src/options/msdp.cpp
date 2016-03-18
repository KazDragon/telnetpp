#include "telnetpp/options/msdp.hpp"

namespace telnetpp { namespace options { namespace msdp {

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
    
}}}
