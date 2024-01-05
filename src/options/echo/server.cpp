#include "telnetpp/options/echo/server.hpp"

namespace telnetpp::options::echo {

server::server(telnetpp::session &sess) noexcept : basic_server(sess)
{
}

}  // namespace telnetpp::options::echo
