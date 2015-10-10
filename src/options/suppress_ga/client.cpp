#include "telnetpp/options/suppress_ga/client.hpp"
#include "telnetpp/options/suppress_ga.hpp"

namespace telnetpp { namespace options { namespace suppress_ga {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
client::client()
  : client_option(telnetpp::options::suppress_ga::option)
{
}

}}}
