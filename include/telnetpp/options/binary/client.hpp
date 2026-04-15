#pragma once

#include "telnetpp/core.hpp"
#include "telnetpp/options/basic_client.hpp"
#include "telnetpp/options/binary/detail/protocol.hpp"

namespace telnetpp::options::binary {

//* =========================================================================
/// \class telnetpp::options::binary::client
/// \extends telnetpp::client_option
/// \brief An implementation of the client side of Telnet binary option
/// \see https://tools.ietf.org/html/rfc857
//* =========================================================================
class TELNETPP_EXPORT client : public telnetpp::options::basic_client<
                                   telnetpp::options::binary::detail::option>
{
public:
    explicit client(telnetpp::session &sess) noexcept;
};

}  // namespace telnetpp::options::binary
