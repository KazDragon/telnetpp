#pragma once

#include "telnetpp/generator.hpp"
#include "telnetpp/parser.hpp"
#include "telnetpp/detail/lambda_visitor.hpp"
#include "telnetpp/client_option.hpp"
#include "telnetpp/server_option.hpp"
#include "telnetpp/detail/command_router.hpp"
#include "telnetpp/detail/negotiation_router.hpp"
#include "telnetpp/detail/subnegotiation_router.hpp"
#include <string>

namespace telnetpp {

//* =========================================================================
/// \brief An abstraction for a Telnet session.
/// \par Overview
/// The session is the heart of the library, and ties all of the other
/// components together.  It manages all of the options that a session
/// wishes to implement and ensures that data flows to them appropriately.
///
/// \par Sending and Receiving Plain Data
///
/// The first part of using a telnetpp::session is understanding how to send
/// and receive data.  Even if no options are used during a session, this is
/// still necessary because some data needs special handling.
///
/// \note According to the Telnet protocol, the 0xFF byte has to be transmitted
/// as IAC IAC (0xFF 0xFF) so that it is not understood to be part of a Telnet
/// protocol sequence.  Likewise, it is necessary to parse IAC IAC into a 
/// single 0xFF byte.
///
/// Sending data uses the telnetpp::session::send function.  This handles any
/// such special sequences and then passes that data onto the continuation
/// supplied.  Example:
///
/// \code
/// // A user-specified function for sending bytes to the remote.
/// void my_socket_send(telnetpp::bytes data);
///
/// telnetpp::session session;
/// session.send(
///     message,
///     [](telnetpp::bytes data)
///     {
///         using telnetpp::literals;
///         auto const message = "Hello" "\xFF" "World"_tb;
///
///         // Sends "Hello\xFF\xFFWorld" to socket_send.
///         // Notice the duplicated \xFF byte as required by the Telnet 
///         //protocol.
///         my_socket_send(data);
///     });
/// \endcode
/// 
/// \note telnetpp::session::send takes a telnetpp::element as its first
/// parameter, but that can be implicitly constructed from the message type
/// shown above.
///
/// \par
///
/// Receiving data works in a similar way, but it is important to note that
/// any data received could conceivably require that data be sent, and so
/// a send continuation similar to the one above must also be attached.
/// Example:
///
/// \code
/// // A user-specified function for sending bytes to the remote.
/// void my_socket_send(telnetpp::bytes data);
///
/// // A user-specified function used for receiving bytes sent from the
/// // remote.
/// int my_socket_receive(telnetpp::byte *buffer, int size);
///
/// // A user-specified function that transmits data up to the application
/// // Note that the second argument is used to tell the application how
/// // it may send a respond to the data it receives.
/// void my_application_receive(
///     telnetpp::bytes data,
///     std::function<void (telnetpp::bytes)> const &send);
///
/// telnetpp::session session;
///
/// telnetpp::byte my_buffer[1024];
/// int amount_received = my_socket_receive(my_buffer, 1024);
///
/// session.receive(
///     telnetpp::bytes{my_buffer, amount_received},
///     // Receive continuation - how to pass on non-Telnet data
///     // to an upper layer
///     my_application_receive,
///     // Send continuation - how to send responses to the
///     // messages received.
///     my_socket_send);
///
/// \endcode
///
/// \par Using Telnet Options
/// 
/// Now that we can send and receive data over a Telnet connection, the next
/// step is to install some options.  This allows the session to automatically
/// route messages to and from these options as part of the normal data flow.  
/// Example:
///
/// \code
/// telnetpp::options::echo::client echo_client;
/// telnetpp::options::naws::server naws_server;
///
/// telnetpp::session session;
/// session.install(echo_client);
/// session.install(naws_server);
/// \endcode
///
/// With these options installed, the normal message as implemented above
/// will automatically activate or deactivate the options if this is
/// requested by the remote.  It is, of course, just fine if you want to
/// activate these options yourself.  Because this necessarily involves
/// transmitting Telnet protocol data, we can forward the responses via
/// the session object and the code we developed during sending and 
/// receiving above.  Example:
///
/// \code
/// naws_server.activate(
///     [&](telnetpp::element const &elem)
///     {
///         // Will transmit IAC WILL NAWS to the remote
///         session.send(elem, my_socket_send);
///     });
/// \endcode
///
/// Likewise, any functions that cause option-specific negotiation
/// (subnegotiations) to occur use a similar pattern:
///
/// \code
/// naws_server.set_window_size(
///     80, 24,
///     [&](telnetpp::element const &elem)
///     {
///         // Will transmit IAC SB NAWS 00 80 00 24 IAC SE to the
///         // remote, assuming that the option is active.
///         session.send(elem, my_socket_send);
///     });
/// \endcode
///
/// \par Using Telnet Commands
///
/// There also exist a small number of Telnet commands, such as AYT (Are You
/// There), BRK (Break) and NOP (No Operation).  These can be sent using the
/// telnetpp::session::send function in a similar way to plain text.  Example:
///
/// \code
/// session.send(telnetpp::command{telnetpp::ayt}, my_socket_send);
/// \endcode
///
/// They can also have handlers registered in order to respond to them:
///
/// \code
/// session.install(
///     telnetpp::command{telnetpp::ayt},
///     [&]()
///     {
///         using telnetpp::literals;
///         auto const message = "Yes, I'm here"_tb;
///         session.send(message, my_socket_send);
///     });
/// \endcode
//* =========================================================================
class TELNETPP_EXPORT session
{
public:
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    session();

    //* =====================================================================
    /// \brief Sends a Telnet data element.  Translates the element into a
    /// sequence of bytes, that is then sent to the continuation.
    /// \param content The data to send
    /// \param send_continuation A continuation that is used to send the 
    /// resultant stream of bytes onward.  Matches the signature
    /// `void (telnetpp::bytes)`.
    //* =====================================================================
    template <typename SendContinuation>
    void send(
        telnetpp::element const &elem,
        SendContinuation &&send_continuation)
    {
        telnetpp::generate(elem, send_continuation);
    }

    //* =====================================================================
    /// \brief Receive a stream of bytes.
    /// \param content The content of the data.
    /// \param receive_continuation A continuation for any data received that
    /// is not encapsulated as Telnet protocol units (i.e. plain text).  
    /// Matches the signature `void (telnetpp::bytes, SendContinuation &&)`.
    /// \param send_continuation A continuation that is used to send any 
    /// response that occur as a consequence of receiving the data. Matches
    /// the signature `void (telnetpp::bytes)`.
    //* =====================================================================
    template <typename ReceiveContinuation, typename SendContinuation>
    void receive(
        telnetpp::bytes content,
        ReceiveContinuation &&receive_continuation,
        SendContinuation &&send_continuation)
    {
        // TODO: The vast majority of the time, a packet will be completely 
        // parsed and require no temporary storage.  Optimize for that case.
        unparsed_buffer_.insert(
            unparsed_buffer_.end(),
            content.begin(),
            content.end());

        auto remainder = telnetpp::parse(
            unparsed_buffer_,
            [&](telnetpp::element const &elem)
            {
                auto generator = [&](telnetpp::element const &elem)
                {
                    telnetpp::generate(elem, send_continuation);
                };

                detail::visit_lambdas(
                    elem,
                    [&](telnetpp::bytes content)
                    {
                        receive_continuation(
                            content, 
                            [&](telnetpp::bytes data)
                            {
                                generator(data);
                            });
                    },
                    [&](telnetpp::command const &cmd)
                    {
                        command_router_(cmd, generator);
                    },
                    [&](telnetpp::negotiation const &neg)
                    {
                        negotiation_router_(neg, generator);
                    },
                    [&](telnetpp::subnegotiation const &sub)
                    {
                        subnegotiation_router_(sub, generator);
                    });
            });

        unparsed_buffer_.erase(0, unparsed_buffer_.size() - remainder.size());
    }

    //* =====================================================================
    /// \brief Installs a handler for the given command.
    //* =====================================================================
    template <typename Continuation>
    void install(telnetpp::command_type cmd, Continuation &&cont)
    {
        command_router_.register_route(cmd, cont);
    }

    //* =====================================================================
    /// \brief Installs a client option.
    //* =====================================================================
    void install(client_option &option);

    //* =====================================================================
    /// \brief Installs a server option.
    //* =====================================================================
    void install(server_option &option);

private:
    std::basic_string<telnetpp::byte>       unparsed_buffer_;
    telnetpp::detail::command_router        command_router_;
    telnetpp::detail::negotiation_router    negotiation_router_;
    telnetpp::detail::subnegotiation_router subnegotiation_router_;
};

}
