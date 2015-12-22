#include "telnetpp/options/new_environ/server.hpp"
#include "telnetpp/options/new_environ.hpp"

namespace telnetpp { namespace options { namespace new_environ {
    
// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
server::server()
  : server_option(telnetpp::options::new_environ::option)
{
    
}

// ==========================================================================
// REQUEST
// ==========================================================================
std::vector<token> server::request_variables(std::vector<request> const &requests)
{
    return {
        telnetpp::element { 
            telnetpp::subnegotiation(
                option(), 
                { telnetpp::options::new_environ::send })
        }
    };
}

}}}
