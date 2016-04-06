#pragma once

#include "telnetpp/options/subnegotiationless_server.hpp"
#include "telnetpp/options/suppress_ga/detail/protocol.hpp"

namespace telnetpp { namespace options { namespace suppress_ga {

using server = telnetpp::options::subnegotiationless_server<
    telnetpp::options::suppress_ga::detail::option
>;

}}}
