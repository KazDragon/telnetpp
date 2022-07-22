#include "telnetpp/options/suppress_ga/client.hpp"

namespace telnetpp { namespace options { namespace suppress_ga {

client::client(telnetpp::session &sess)
  : basic_client(sess)
{
}

}}}
