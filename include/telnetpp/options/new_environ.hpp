#pragma once

#include "telnetpp/core.hpp"
#include <boost/optional.hpp>
#include <string>

namespace telnetpp { namespace options { namespace new_environ {

static constexpr telnetpp::u8 option  = 39;

static constexpr telnetpp::u8 is      = 0;
static constexpr telnetpp::u8 send    = 1;
static constexpr telnetpp::u8 info    = 2;

static constexpr telnetpp::u8 var     = 0;
static constexpr telnetpp::u8 value   = 1;
static constexpr telnetpp::u8 esc     = 2;
static constexpr telnetpp::u8 uservar = 3;

struct request
{
    telnetpp::u8 type;
    std::string  name;
};

struct response
{
    u8                           type;
    std::string                  name;
    boost::optional<std::string> value;
};

}}}
