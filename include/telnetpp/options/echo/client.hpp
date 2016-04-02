#ifndef TELNETPP_OPTIONS_ECHO_CLIENT
#define TELNETPP_OPTIONS_ECHO_CLIENT

#include "telnetpp/options/subnegotiationless_client.hpp"
#include "telnetpp/options/echo.hpp"

namespace telnetpp { namespace options { namespace echo {

using client = telnetpp::options::subnegotiationless_client<
    telnetpp::options::echo::option
>;

}}}

#endif
