#pragma once

#include "telnetpp/client_option.hpp"
#include "telnetpp/server_option.hpp"
#include "telnetpp/detail/routing_visitor.hpp"
#include <functional>

namespace telnetpp {

//* =========================================================================
/// \brief An abstraction for a Telnet session.
/// \par Overview
/// The session is the heart of the library, and ties all of the other
/// components together.  It manages all of the options that a session
/// wishes to implement and ensures that data flows to them as appropriate.
/// \par Construction
/// Session's constructor takes one parameter, which is a function that
/// takes a string and returns a vector of tokens.  This function is called
/// when data has been received by the session and did not belong to the
/// Telnet protocol, i.e. application data.  In practice, this is where most
/// of the dataflow of an application takes place.  It's possible that there
/// is an immediate response to this data, and that is encapsulated in the
/// return value.  Note that the return value can include Telnet elements.
/// Example:
/// \code
///     std::vector<telnetpp::token> app_fn(std::string const &text)
///     {
///         if (text == "COMPRESS")
///         {
///             // This MCCP option returns a vector<token>
///             return mccp_server.begin_compression();
///         }
///         else if (text == "ECHO")
///         {
///             // Just return some string to be sent.
///             return { telnetpp::element("Echo!") };
///         }
///         ...
///     }
///
///     ...
///
///     telnetpp::session session(app_fn);
/// \endcode
/// \par Installation
/// It is possible to "install" handlers into the session.  The most basic
/// installation is for handling Telnet commands, such as "Are You There?".
/// It is also possible to install client- and server options into the
/// session, which ensures that any negotiations or subnegotiations that are
/// tagged as belonging to those options are forwarded to those option
/// handlers.
/// \code
///     std::vector<telnetpp::token> ayt_handler(telnetpp::command const &)
///     {
///         return { telnetpp::element("Yes, I'm here.") };
///     }
///
///     telnetpp::options::echo::server echo_server;
///
///     ...
///
///     session.install(telnetpp::ayt, ayt_handler);
///     session.install(echo_server);
/// \endcode
/// \par Sending
/// The send() function is used to convert all telnetpp::elements contained
/// in the tokens into sequences of bytes.  Note: even if you know the tokens
/// contain only plain strings, they must still pass through this function,
/// since it's necessary to escape any 0xFF bytes so that they are not
/// received as Telnet commands by the remote side.  After this operation is
/// complete, the application must write the byte streams over the actual
/// data connection itself.
/// \par Receiving
/// The receive() function is used to receive data that has been read from
/// the remote connection and route it to the correct handler, be it the
/// installed options or command handlers, or the text handler registered in
/// the constructor.  Note: receiving data frequently has an immediate
/// response.  For example, receving bytes that amount to an option
/// negotiation will result in a reply to that negotiation.  For this reason,
/// the usual pattern for receiving is to immediately send() the result of
/// the call to receive, and then write that result to the data connection.
/// \code
///     void recv(telnetpp::byte_stream const &data)
///     {
///         my_lower_layer_write(
///             session.send(session.receive(data)));
///     }
/// \endcode
//* =========================================================================
class TELNETPP_EXPORT session
{
public :
    //* =====================================================================
    /// \brief Constructor
    /// \param on_text a function to be called whenever text is received.
    //* =====================================================================
    explicit session(
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
    std::vector<token> receive(byte_stream const &stream);

    //* =====================================================================
    /// \brief "Sends" a stream of tokens by converting them to a stream of
    /// bytes.  Any non-element tokens are passed through unchanged.  This
    /// allows the result of receive() to be passed straight back to
    /// generate for immediate transmission.
    //* =====================================================================
    std::vector<stream_token> send(std::vector<token> const &tokens);

private :
    byte_stream                             unparsed_buffer_;
    telnetpp::detail::command_router        command_router_;
    telnetpp::detail::negotiation_router    negotiation_router_;
    telnetpp::detail::subnegotiation_router subnegotiation_router_;
    telnetpp::detail::routing_visitor       visitor_;

};

}
