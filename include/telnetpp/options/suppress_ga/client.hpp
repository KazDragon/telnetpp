#ifndef TELNETPP_OPTIONS_SUPPRESS_GA_CLIENT
#define TELNETPP_OPTIONS_SUPPRESS_GA_CLIENT

#include "telnetpp/client_option.hpp"

namespace telnetpp { namespace options { namespace suppress_ga {

//* =========================================================================
/// \brief An implementation of the client side of the Telnet echo option.
//* =========================================================================
class client : public telnetpp::client_option {
public :
    //* =====================================================================
    /// CONSTRUCTOR
    //* =====================================================================
    client();
};

}}}

#endif
