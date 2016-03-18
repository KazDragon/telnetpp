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

struct variable;

using value_type = boost::variant<
    std::string, 
    std::vector<std::string>,
    boost::recursive_wrapper<std::vector<variable>>
>;

struct TELNETPP_EXPORT variable 
{
    variable(
        std::string const &name,
        value_type const &value);
        
    variable(
        std::string const &name, 
        std::initializer_list<std::string> const &array_values);
        
    variable(
        std::string const &name,
        std::initializer_list<variable> const &table_values);

    std::string name;
    value_type  value;
};

}}}

#endif
