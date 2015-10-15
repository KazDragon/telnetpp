#ifndef TELNETPP_SESSION_HPP_
#define TELNETPP_SESSION_HPP_

#include "telnetpp/client_option.hpp"
#include "telnetpp/server_option.hpp"
#include "telnetpp/routing_visitor.hpp"
#include <functional>

namespace telnetpp {

//* =========================================================================
/// \brief An abstraction for a Telnet session.
//* =========================================================================
class session
{
public :
    //* =====================================================================
    /// \brief Constructor
    /// \param on_text a function to be called whenever text is received.
    //* =====================================================================
    session(
        std::function<std::vector<token> (std::string const&)> on_text);
    
    //* =====================================================================
    /// \brief Installs a client option.
    //* =====================================================================
    void install(client_option &option);
    
    //* =====================================================================
    /// \brief Installs a server option.
    //* =====================================================================
    void install(server_option &option);
    
    //* =====================================================================
    /// \brief Receive a stream of bytes.
    /// \returns a stream of tokens generated as a result of receiving this
    ///          stream.
    //* =====================================================================
    std::vector<token> receive(u8stream const &stream);
    
private :
    u8stream              unparsed_buffer_;
    command_router        command_router_;
    negotiation_router    negotiation_router_;
    subnegotiation_router subnegotiation_router_;
    routing_visitor       visitor_;
    
};

}

#endif