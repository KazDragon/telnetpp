#include "telnetpp/options/echo/server.hpp"

namespace telnetpp { namespace options { namespace echo {

server::server(telnetpp::session &sess) noexcept
  : basic_server(sess)
{
}

}}}
