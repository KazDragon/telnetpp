#include "telnetpp/options/binary/server.hpp"

namespace telnetpp::options::binary {

server::server(telnetpp::session &sess) noexcept : basic_server(sess)
{
}

}  // namespace telnetpp::options::binary
