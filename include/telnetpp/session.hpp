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
    /// resultant stream of bytes onward.
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
    /// \param data_continuation A continuation for any data received that is
    /// not encapsulated as Telnet protocol units (i.e. plain text).  Matches
    /// the signature `void (telnetpp::bytes)`.
    /// \param send_continuation A continuation that is used to send any 
    /// response that occur as a consequence of receiving the data. Matches
    /// the signature `void (telnetpp::element)`.
    //* =====================================================================
    template <typename DataContinuation, typename SendContinuation>
    void receive(
        telnetpp::bytes content,
        DataContinuation &&data_continuation,
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
                        data_continuation(content, generator);
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
    void install(
        telnetpp::command_type cmd,
        Continuation &&cont)
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
