#pragma once

#include "telnetpp/client_option.hpp"
#include <boost/signals2/signal.hpp>
#include <vector>

namespace telnetpp { namespace options { namespace new_environ {

//* =========================================================================
/// \brief An enumeration of the type of variables that NEW_ENVIRON handles.
//* =========================================================================
enum class variable_type
{
    var,
    uservar
};

//* =========================================================================
/// \brief A request that is made of the remote server.
//* =========================================================================
struct request
{
    variable_type type;
    std::string   name;
};

//* =========================================================================
/// \brief A response that is received from the remote server.
//* =========================================================================
struct response
{
    variable_type                type;
    std::string                  name;
    boost::optional<std::string> value;
};

//* =========================================================================
/// \brief An implementation of the client side of the Telnet New-Environ
/// option.
//* =========================================================================
class TELNETPP_EXPORT client : public telnetpp::client_option
{
public :
    //* =====================================================================
    /// CONSTRUCTOR
    //* =====================================================================
    client();

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
        std::vector<token> (response const &res),
        telnetpp::token_combiner
    > on_variable_changed;

private :
    //* =====================================================================
    /// \brief Handle a negotiation that has been received in the active
    /// state.
    //* =====================================================================
    std::vector<telnetpp::token> handle_subnegotiation(u8stream const &content);

};

}}}
