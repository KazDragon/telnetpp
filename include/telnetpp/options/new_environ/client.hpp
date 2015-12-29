#ifndef TELNETPP_OPTIONS_NEW_ENVIRON_CLIENT_HPP_
#define TELNETPP_OPTIONS_NEW_ENVIRON_CLIENT_HPP_

#include "telnetpp/client_option.hpp"
#include <map>

namespace telnetpp { namespace options { namespace new_environ {

//* =========================================================================
/// \brief An implementation of the client side of the Telnet New-Environ
/// option.
//* =========================================================================
class TELNETPP_EXPORT client : public client_option
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    client();

    //* =====================================================================
    /// \brief Sets a "VAR" type variable in the environment.
    //* =====================================================================
    void set_variable(std::string const &name, std::string const &value);
    
private :
    //* =====================================================================
    /// \brief Handle a negotiation that has been received in the active
    /// state.
    //* =====================================================================
    std::vector<telnetpp::token> handle_subnegotiation(u8stream const &content);

    std::map<std::string, std::string> variables_;
};

}}}

#endif