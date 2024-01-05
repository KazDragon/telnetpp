#include "telnetpp/options/suppress_ga/server.hpp"

namespace telnetpp::options::suppress_ga {

server::server(telnetpp::session &sess) noexcept : basic_server(sess)
{
}

}  // namespace telnetpp::options::suppress_ga
