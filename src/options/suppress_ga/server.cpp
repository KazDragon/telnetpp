#include "telnetpp/options/suppress_ga/server.hpp"
#include "telnetpp/options/suppress_ga.hpp"

namespace telnetpp { namespace options { namespace suppress_ga {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
server::server()
  : server_option(telnetpp::options::suppress_ga::option)
{
}

}}}
