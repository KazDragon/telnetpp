#ifndef TELNETPP_OPTIONS_MSDP_HPP_
#define TELNETPP_OPTIONS_MSDP_HPP_

#include "telnetpp/core.hpp"
#include <boost/variant.hpp>
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

struct variable 
{
    std::string name;
    std::string value;
};

}}}

#endif
