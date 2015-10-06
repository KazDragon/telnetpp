#include "telnetpp/options/echo/client.hpp"
#include "telnetpp/options/echo.hpp"

namespace telnetpp { namespace options { namespace echo {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
client::client()
  : client_option(telnetpp::options::echo::option)
{
}

}}}
