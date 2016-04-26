#pragma once

#include "telnetpp/server_option.hpp"

namespace telnetpp { namespace options { namespace mccp {

//* =========================================================================
/// \brief A server option responsible for negotiating the server part of the
/// MCCP protocol.
//* =========================================================================
class TELNETPP_EXPORT server : public telnetpp::server_option
{
public :
    server();
};

}}}
