#pragma once

#include "telnetpp/server_option.hpp"
#include <boost/optional.hpp>
#include <map>

namespace telnetpp { namespace options { namespace new_environ {

//* =========================================================================
/// \brief An implementation of the server side of the Telnet New-Environ
/// option.
//* =========================================================================
class TELNETPP_EXPORT server : public server_option
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    server();

    //* =====================================================================
    /// \brief Sets a "VAR" type variable in the environment.
    //* =====================================================================
    std::vector<telnetpp::token> set_variable(
        std::string const &name, std::string const &value);

    //* =====================================================================
    /// \brief Deletes a "VAR" type variable from the environment.
    //* =====================================================================
    std::vector<telnetpp::token> delete_variable(std::string const &name);

    //* =====================================================================
    /// \brief Sets a "USERVAR" type variable in the environment.
    //* =====================================================================
    std::vector<telnetpp::token> set_user_variable(
        std::string const &name, std::string const &value);

    //* =====================================================================
    /// \brief Deletes a "USERVAR" type variable from the environment.
    //* =====================================================================
    std::vector<telnetpp::token> delete_user_variable(std::string const &name);

private :
    //* =====================================================================
    /// \brief Handle a negotiation that has been received in the active
    /// state.
    //* =====================================================================
    std::vector<telnetpp::token> handle_subnegotiation(u8stream const &content);

    std::map<std::string, std::string> variables_;
    std::map<std::string, std::string> user_variables_;
};

}}}
