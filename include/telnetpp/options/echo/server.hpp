#pragma once

#include "telnetpp/core.hpp"
#include "telnetpp/options/basic_server.hpp"
#include "telnetpp/options/echo/detail/protocol.hpp"

namespace telnetpp::options::echo {

//* =========================================================================
/// \class telnetpp::options::echo::server
/// \extends telnetpp::server_option
/// \brief An implementation of the server side of Telnet ECHO option
/// \see https://tools.ietf.org/html/rfc857
//* =========================================================================
class TELNETPP_EXPORT server : public telnetpp::options::basic_server<
                                   telnetpp::options::echo::detail::option>
{
 public:
  constexpr explicit server(telnetpp::session &sess) noexcept
    : basic_server(sess)
  {
  }
};

}  // namespace telnetpp::options::echo
