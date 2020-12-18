#pragma once

#include "telnetpp/core.hpp"
#include "telnetpp/options/basic_client.hpp"
#include "telnetpp/options/echo/detail/protocol.hpp"

namespace telnetpp { namespace options { namespace echo {

//* =========================================================================
/// \class telnetpp::options::echo::client
/// \extends telnetpp::client_option
/// \brief An implementation of the client side of Telnet ECHO option
/// \see https://tools.ietf.org/html/rfc857
//* =========================================================================
class TELNETPP_EXPORT client : public telnetpp::options::basic_client<
    telnetpp::options::echo::detail::option
>
{
public:
    //client();
};

}}}
