#include "telnetpp/options/suppress_ga/server.hpp"

namespace telnetpp { namespace options { namespace suppress_ga {

server::server(telnetpp::session &sess) noexcept
  : basic_server(sess)
{
}

}}}
