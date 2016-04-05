#pragma once

#include "telnetpp/options/subnegotiationless_client.hpp"
#include "telnetpp/options/echo/detail/echo.hpp"

namespace telnetpp { namespace options { namespace echo {

using client = telnetpp::options::subnegotiationless_client<
    telnetpp::options::echo::detail::option
>;

}}}
