#include "telnetpp/options/echo/client.hpp"

namespace telnetpp::options::echo {

client::client(telnetpp::session &sess) noexcept : basic_client(sess)
{
}

}  // namespace telnetpp::options::echo
