#ifndef TELNETPP_OPTIONS_MSDP_HPP_
#define TELNETPP_OPTIONS_MSDP_HPP_

#include "telnetpp/core.hpp"
#include <boost/variant.hpp>
#include <initializer_list>
#include <string>
#include <vector>

namespace telnetpp { namespace options { namespace msdp {
   
static constexpr telnetpp::u8 option = 69;

static constexpr telnetpp::u8 var         = 1;
static constexpr telnetpp::u8 val         = 2;
static constexpr telnetpp::u8 table_open  = 3;
static constexpr telnetpp::u8 table_close = 4;
static constexpr telnetpp::u8 array_open  = 5;
static constexpr telnetpp::u8 array_close = 6;

using value_type = boost::variant<std::string, std::vector<std::string>>;

struct variable 
{
    std::string name;
    value_type  value;
};

TELNETPP_EXPORT
value_type make_value(std::initializer_list<std::string> const &il);

}}}

#endif
