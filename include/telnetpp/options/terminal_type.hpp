#ifndef TELNET_OPTIONS_TERMINAL_TYPE_HPP_
#define TELNET_OPTIONS_TERMINAL_TYPE_HPP_

#include "telnetpp/core.hpp"

namespace telnetpp { namespace options { namespace terminal_type {
   
static constexpr telnetpp::u8 option = 24;
static constexpr telnetpp::u8 is     = 0;
static constexpr telnetpp::u8 send   = 1;

}}}

#endif
