#pragma once

#include "telnetpp/core.hpp"
#include "telnetpp/options/basic_server.hpp"
#include "telnetpp/options/binary/detail/protocol.hpp"

namespace telnetpp::options::binary {

//* =========================================================================
/// \class telnetpp::options::binary::server
/// \extends telnetpp::server_option
/// \brief An implementation of the server side of Telnet binary option
/// \see https://tools.ietf.org/html/rfc857
//* =========================================================================
class TELNETPP_EXPORT server : public telnetpp::options::basic_server<
                                   telnetpp::options::binary::detail::option>
{
public:
    explicit server(telnetpp::session &sess) noexcept;
};

}  // namespace telnetpp::options::binary
