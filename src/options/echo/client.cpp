#include "telnetpp/options/echo/client.hpp"

namespace telnetpp { namespace options { namespace echo {

client::client(telnetpp::session &sess) noexcept
  : basic_client(sess)
{
}

}}}
