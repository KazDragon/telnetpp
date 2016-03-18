#include "telnetpp/options/msdp.hpp"

namespace telnetpp { namespace options { namespace msdp {
    
value_type make_value(std::string const &value)
{
    return value;
}

value_type make_value(std::initializer_list<std::string> const &il)
{
    return std::vector<std::string>(il);
}

value_type make_value(std::initializer_list<variable> const &il)
{
    return value_type(std::vector<variable>(il));
}

}}}
