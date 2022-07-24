#pragma once

#include <telnetpp/core.hpp>
#include <telnetpp/element.hpp>
#include <functional>
#include <memory>

namespace telnetpp {

class client_option;
class server_option;

//* =========================================================================
/// \brief An abstraction for a Telnet session.
/// \par Overview
/// The session is the heart of the library, and ties all of the other
/// components together.  It manages all of the options that a session
/// wishes to implement and ensures that data flows to them appropriately.
///
/// \par The Channel
/// To be as agnostic as possible as to how to send and receive data, the
/// session exposes a channel concept, which is fulfilled by any object
/// that contains the functions: write, async_read, is_alive, close.
/// Classes that expose these functions, such as tcp_socket in Server++,
/// can be dropped in here without any extra work.  Otherwise it may be
/// necessary to write a small wrapper.
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
/// Sending data uses the telnetpp::session::write function.  This handles any
/// such special sequences and then passes that data onto the continuation
/// supplied.  Example:
///
/// \code
/// using namespace terminalpp::literals;
/// struct my_channel
/// {
///     void async_read(std::function<void (telnetpp::bytes)>);
///     void write(telnetpp::bytes);
///     // ...
/// };
///
/// my_channel channel;
/// telnetpp::session session{channel};
///
/// auto const message = "Hello" "\xFF" "World"_tb;
/// session.send(message);
/// // sends "Hello\xFF\xFFWorld" to channel.write().
/// // Notice the duplicated \xFF byte as required by the Telnet protocol.
/// \endcode
/// 
/// \par
///
/// Receiving data works using a continuation; a function that is called once
/// the data is received.  This means that it is non-blocking.
/// Example:
///
/// \code
/// my_channel channel;
/// telnetpp::session session{channel};
///
/// // A user-specified function that transmits data up to the application
/// // Note that the second argument is used to tell the application how
/// // it may send a respond to the data it receives.
/// void my_application_receive(telnetpp::bytes data);
///
/// session.async_read([&](telnetpp::bytes data) {
///     my_application_receive(data);
/// });
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
/// telnetpp::session session{channel};
/// telnetpp::options::echo::client echo_client{session};
/// telnetpp::options::naws::server naws_server{session};
/// session.install(echo_client);
/// session.install(naws_server);
/// \endcode
///
/// With these options installed, the normal message as implemented above
/// will automatically activate or deactivate the options if this is
/// requested by the remote.  It is, of course, just fine if you want to
/// activate these options yourself. Example:
///
/// \code
/// Transmits IAC WILL NAWS to the remote.
/// naws_server.activate();
/// \endcode
///
/// Likewise, any functions that cause option-specific negotiation
/// (subnegotiations) to occur use a similar pattern:
///
/// \code
/// // Will transmit IAC SB NAWS 00 80 00 24 IAC SE to the
/// // remote.
/// naws_server.set_window_size(80, 24);
/// \endcode
///
/// \par Using Telnet Commands
///
/// There also exist a small number of Telnet commands, such as AYT (Are You
/// There), BRK (Break) and NOP (No Operation).  These can be sent using the
/// telnetpp::session::send function in a similar way to plain text.  Example:
///
/// \code
/// session.write(telnetpp::command{telnetpp::ayt});
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
///         session.write(message);
///     });
/// \endcode
//* =========================================================================
class TELNETPP_EXPORT session final
{
public:
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    template <typename Channel>
    explicit session(Channel &channel)
      : session{}
    {
        channel_ = std::unique_ptr<channel_concept>(
            std::make_unique<channel_model<Channel>>(channel));
    }

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    ~session();

    //* =====================================================================
    /// \brief Returns whether the session is still alive
    //* =====================================================================
    bool is_alive() const;

    //* =====================================================================
    /// \brief Closes the session.
    //* =====================================================================
    void close();

    //* =====================================================================
    /// \brief Requests data from the underlying channel, acting on any
    /// Telnet primitives that are received.
    ///
    /// Note: the callback may be called several times for any async_read.
    /// For an example, it may contain two pieces of regular text surrounding
    /// a block of negotiation, which may change the way those two pieces of
    /// text are interpreted.  For this reason, after an async_read is 
    /// complete, the callback will always be called with an empty parameter
    /// to indicate that a new read request can be made.
    //* =====================================================================
    void async_read(std::function<void (telnetpp::bytes)> const &callback);

    //* =====================================================================
    /// \brief Sends a Telnet data element.  Translates the element into a
    /// sequence of bytes, that is then sent to the continuation.
    /// \param content The data to send
    //* =====================================================================
    void write(telnetpp::element const &elem);

    //* =====================================================================
    /// \brief Installs a handler for the given command.
    //* =====================================================================
    void install(
        telnetpp::command_type cmd, 
        std::function<void (telnetpp::command)> const &handler);

    //* =====================================================================
    /// \brief Installs a client option.
    //* =====================================================================
    void install(telnetpp::client_option &option);

    //* =====================================================================
    /// \brief Installs a server option.
    //* =====================================================================
    void install(telnetpp::server_option &option);

private:
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    session();

    //* =====================================================================
    /// \brief An interface for the channel model.
    //* =====================================================================
    struct channel_concept
    {
        //* =================================================================
        /// \brief Destructor
        //* =================================================================
        virtual ~channel_concept() = default;

        //* =================================================================
        /// \brief Asynchronously read from the channel and call the function
        /// back when it's available.
        //* =================================================================
        virtual void async_read(std::function<void (bytes)> const &) = 0;

        //* =================================================================
        /// \brief Write the given data to the channel.
        //* =================================================================
        virtual void write(bytes data) = 0;

        //* =================================================================
        /// \brief Returns whether the channel is alive.
        //* =================================================================
        virtual bool is_alive() const = 0;

        //* =================================================================
        /// \brief Closes the channel.
        //* =================================================================
        virtual void close() = 0;
    };

    template <typename Channel>
    struct channel_model final : channel_concept
    {
        //* =================================================================
        /// \brief Constructor
        //* =================================================================
        explicit channel_model(Channel &channel)
          : channel_{channel}
        {
        }

        //* =================================================================
        /// \brief Asynchronously read from the channel and call the function
        /// back when it's available.
        //* =================================================================
        void async_read(std::function<void (bytes)> const &callback)
        {
            channel_.async_read(callback);
        }

        //* =================================================================
        /// \brief Write the given data to the channel.
        //* =================================================================
        void write(bytes data)
        {
            channel_.write(data);
        }

        //* =================================================================
        /// \brief Returns whether the channel is alive.
        //* =================================================================
        bool is_alive() const
        {
            return channel_.is_alive();
        }

        //* =================================================================
        /// \brief Closes the channel.
        //* =================================================================
        void close()
        {
            channel_.close();
        }

        Channel &channel_;
    };

    struct impl;
    std::unique_ptr<channel_concept> channel_;
    std::unique_ptr<impl> pimpl_;
};

}
