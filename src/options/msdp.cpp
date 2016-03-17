#include "telnetpp/options/msdp.hpp"

namespace telnetpp { namespace options { namespace msdp {
    
value_type make_value(std::initializer_list<std::string> const &il)
{
    return std::vector<std::string>(il);
}

}}}
