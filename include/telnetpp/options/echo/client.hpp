#ifndef TELNETPP_OPTIONS_ECHO_CLIENT
#define TELNETPP_OPTIONS_ECHO_CLIENT

#include "telnetpp/client_option.hpp"

namespace telnetpp { namespace options { namespace echo {

class client : public telnetpp::client_option {
public :
    //* =====================================================================
    /// CONSTRUCTOR
    //* =====================================================================
    client();
};

}}}

#endif
