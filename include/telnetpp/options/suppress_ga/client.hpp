#pragma once

#include "telnetpp/core.hpp"
#include "telnetpp/options/basic_client.hpp"
#include "telnetpp/options/suppress_ga/detail/protocol.hpp"

namespace telnetpp::options::suppress_ga {

//* =========================================================================
/// \class telnetpp::options::suppress_ga::client
/// \extends telnetpp::client_option
/// \brief An implementation of the client side of the Telnet Suppress Go-
/// Ahead option.
/// \see https://tools.ietf.org/html/rfc858
//* =========================================================================
class TELNETPP_EXPORT client
  : public telnetpp::options::basic_client<
        telnetpp::options::suppress_ga::detail::option>
{
public:
    explicit client(telnetpp::session &sess) noexcept;
};

}  // namespace telnetpp::options::suppress_ga
