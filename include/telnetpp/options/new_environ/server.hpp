#ifndef TELNETPP_OPTIONS_NEW_ENVIRON_SERVER_HPP_
#define TELNETPP_OPTIONS_NEW_ENVIRON_SERVER_HPP_

#include "telnetpp/server_option.hpp"
#include <boost/optional.hpp>
#include <boost/signals2/signal.hpp>
#include <vector>

namespace telnetpp { namespace options { namespace new_environ {

struct request
{
    telnetpp::u8 type;
    std::string  name;
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

    //* =====================================================================
    /// \brief Signal called whenever an environment variable is updated.
    /// \param type the type of the variable; either var or uservar.
    /// \param name the name of the variable.
    /// \param value the value of the variable, including the possibility of
    ///        it being undefined.
    //* =====================================================================
    boost::signals2::signal<
        std::vector<token> (
            u8                                  type,
            std::string                  const &name,
            boost::optional<std::string> const &value)
    > on_variable_changed; 

private :
    //* =====================================================================
    /// \brief Handle a negotiation that has been received in the active
    /// state.
    //* =====================================================================
    std::vector<telnetpp::token> handle_subnegotiation(u8stream const &content);

};
    
}}}

#endif