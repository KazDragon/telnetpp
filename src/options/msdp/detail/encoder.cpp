#include "telnetpp/options/msdp/detail/encoder.hpp"
#include <algorithm>

namespace telnetpp { namespace options { namespace msdp { namespace detail {
    
namespace {

static telnetpp::u8stream &append_variable(
    telnetpp::u8stream &destination,
    telnetpp::options::msdp::variable const &variable)
{
    using std::begin;
    using std::end;
    
    destination.push_back(telnetpp::options::msdp::var);
    destination.insert(
        end(destination), begin(variable.name), end(variable.name));
    destination.push_back(telnetpp::options::msdp::val);
    destination.insert(
        end(destination), begin(variable.value), end(variable.value));
    
    return destination;
}

}

telnetpp::u8stream encode(
    std::vector<telnetpp::options::msdp::variable> const &variables)
{
    return std::accumulate(
        variables.begin(),
        variables.end(),
        telnetpp::u8stream{},
        append_variable);
}

}}}}
