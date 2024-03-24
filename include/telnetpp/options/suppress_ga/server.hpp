#pragma once

#include "telnetpp/core.hpp"
#include "telnetpp/options/basic_server.hpp"
#include "telnetpp/options/suppress_ga/detail/protocol.hpp"

namespace telnetpp::options::suppress_ga {

//* =========================================================================
/// \class telnetpp::options::suppress_ga::server
/// \extends telnetpp::server_option
/// \brief An implementation of the server side of the Telnet Suppress Go-
/// Ahead option.
/// \see https://tools.ietf.org/html/rfc858
//* =========================================================================
class TELNETPP_EXPORT server
  : public telnetpp::options::basic_server<
        telnetpp::options::suppress_ga::detail::option>
{
public:
    explicit server(telnetpp::session &sess) noexcept;
};

}  // namespace telnetpp::options::suppress_ga
