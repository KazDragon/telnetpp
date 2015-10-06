#include "telnetpp/options/echo/server.hpp"
#include "telnetpp/options/echo.hpp"

namespace telnetpp { namespace options { namespace echo {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
server::server()
  : server_option(telnetpp::options::echo::option)
{
}

}}}
