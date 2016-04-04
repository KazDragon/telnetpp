#pragma once

#include "telnetpp/client_option.hpp"
#include "telnetpp/server_option.hpp"
#include "telnetpp/detail/routing_visitor.hpp"
#include <functional>

namespace telnetpp {

//* =========================================================================
/// \brief An abstraction for a Telnet session.
//* =========================================================================
class TELNETPP_EXPORT session
{
public :
    //* =====================================================================
    /// \brief Constructor
    /// \param on_text a function to be called whenever text is received.
    //* =====================================================================
    session(
        std::function<std::vector<token> (std::string const&)> on_text);

    //* =====================================================================
    /// \brief Installs a handler for the given command.
    //* =====================================================================
    void install(
        command const &cmd,
        std::function<std::vector<token> (command const &)> const &on_command);

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

    //* =====================================================================
    /// \brief "Sends" a stream of tokens by converting them to a stream of
    /// bytes.  Any non-element tokens are passed through unchanged.  This
    /// allows the result of receive() to be passed straight back to
    /// generate for immediate transmission.
    //* =====================================================================
    std::vector<boost::variant<u8stream, boost::any>> send(
        std::vector<token> const &tokens);

private :
    u8stream                                unparsed_buffer_;
    telnetpp::detail::command_router        command_router_;
    telnetpp::detail::negotiation_router    negotiation_router_;
    telnetpp::detail::subnegotiation_router subnegotiation_router_;
    telnetpp::detail::routing_visitor       visitor_;

};

}
