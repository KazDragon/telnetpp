#ifndef TELNETPP_OPTIONS_ECHO_CLIENT
#define TELNETPP_OPTIONS_ECHO_CLIENT

#include "telnetpp/client_option.hpp"

namespace telnetpp { namespace options { namespace echo {

//* =========================================================================
/// \brief An implementation of the client side of the Telnet echo option.
//* =========================================================================
class TELNETPP_EXPORT client : public telnetpp::client_option {
public :
    //* =====================================================================
    /// CONSTRUCTOR
    //* =====================================================================
    client();
};

}}}

#endif
