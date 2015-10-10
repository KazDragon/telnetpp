#ifndef TELNETPP_OPTIONS_SUPPRESS_GA_SERVER_HPP_
#define TELNETPP_OPTIONS_SUPPRESS_GA_SERVER_HPP_

#include "telnetpp/server_option.hpp"

namespace telnetpp { namespace options { namespace suppress_ga {

//* =========================================================================
/// \brief An implementation of the server side of the Telnet echo option.
//* =========================================================================
class server : public telnetpp::server_option {
public :
    //* =====================================================================
    /// CONSTRUCTOR
    //* =====================================================================
    server();
};

}}}

#endif
