#ifndef TELNET_OPTIONS_NEW_ENVIRON_HPP_
#define TELNET_OPTIONS_NEW_ENVIRON_HPP_

#include "telnetpp/core.hpp"

namespace telnetpp { namespace options { namespace new_environ {
   
static constexpr telnetpp::u8 option  = 39;

static constexpr telnetpp::u8 is      = 0;
static constexpr telnetpp::u8 send    = 1;
static constexpr telnetpp::u8 info    = 2;

static constexpr telnetpp::u8 var     = 0;
static constexpr telnetpp::u8 value   = 1;
static constexpr telnetpp::u8 esc     = 2;
static constexpr telnetpp::u8 uservar = 3;

}}}

#endif
