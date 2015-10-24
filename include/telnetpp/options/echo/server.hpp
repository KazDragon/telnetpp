#ifndef TELNETPP_OPTIONS_ECHO_SERVER
#define TELNETPP_OPTIONS_ECHO_SERVER

#include "telnetpp/server_option.hpp"

namespace telnetpp { namespace options { namespace echo {

//* =========================================================================
/// \brief An implementation of the server side of the Telnet echo option.
//* =========================================================================
class TELNETPP_EXPORT server : public telnetpp::server_option {
public :
    //* =====================================================================
    /// CONSTRUCTOR
    //* =====================================================================
    server();
};

}}}

#endif
