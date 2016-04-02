#ifndef TELNETPP_OPTIONS_ECHO_SERVER
#define TELNETPP_OPTIONS_ECHO_SERVER

#include "telnetpp/options/subnegotiationless_server.hpp"
#include "telnetpp/options/echo.hpp"

namespace telnetpp { namespace options { namespace echo {

using server = telnetpp::options::subnegotiationless_server<
    telnetpp::options::echo::option
>;

}}}

#endif
