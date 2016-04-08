#include "telnetpp/options/mccp/server.hpp"
#include "telnetpp/options/mccp/detail/protocol.hpp"

namespace telnetpp { namespace options { namespace mccp {

server::server()
  : server_option(detail::option)
{
}

}}}
