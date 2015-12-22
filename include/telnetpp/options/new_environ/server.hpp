#ifndef TELNETPP_OPTIONS_NEW_ENVIRON_SERVER_HPP_
#define TELNETPP_OPTIONS_NEW_ENVIRON_SERVER_HPP_

#include "telnetpp/server_option.hpp"
#include <vector>

namespace telnetpp { namespace options { namespace new_environ {

struct request
{
    telnetpp::u8 type;
    std::string  name;
};

struct response
{
    telnetpp::u8 type;
    std::string  name;
    std::string  value;
};

//* =========================================================================
/// \brief An implementation of the server side of the Telnet New-Environ
/// option.
//* =========================================================================
class TELNETPP_EXPORT server : public telnetpp::server_option
{
public :
    //* =====================================================================
    /// CONSTRUCTOR
    //* =====================================================================
    server();
    
    //* =====================================================================
    /// \brief Requests that a particular set of environment variables be
    /// transmitted by the client.
    //* =====================================================================
    std::vector<token> request_variables(std::vector<request> const &requests);
};
    
}}}

#endif