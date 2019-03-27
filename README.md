# Telnet++

[![Documentation](https://codedocs.xyz/KazDragon/telnetpp.svg)](https://codedocs.xyz/KazDragon/telnetpp/)
[![License](https://img.shields.io/github/license/KazDragon/telnetpp.svg)](https://en.wikipedia.org/wiki/MIT_License)
[![GitHub Releases](https://img.shields.io/github/release/KazDragon/telnetpp.svg)](https://github.com/KazDragon/telnetpp/releases)
[![Build Status](https://travis-ci.org/KazDragon/telnetpp.svg?branch=master)](https://travis-ci.org/KazDragon/telnetpp)
[![Coverage Status](https://coveralls.io/repos/github/KazDragon/telnetpp/badge.svg?branch=master)](https://coveralls.io/github/KazDragon/telnetpp?branch=master)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/63ec54845f2c41f1899706c61f1c316b)](https://www.codacy.com/app/KazDragon/telnetpp?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=KazDragon/telnetpp&amp;utm_campaign=Badge_Grade)

[![Github Issues](https://img.shields.io/github/issues/KazDragon/telnetpp.svg)](https://github.com/KazDragon/telnetpp/issues)

[![Gitter](https://badges.gitter.im/KazDragon/telnetpp.svg)](https://gitter.im/KazDragon/telnetpp?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)

Telnet++ is an implementation of the Telnet Session Layer protocol that is used primarily to negotiate a feature set between a client and server, the former of which is usually some kind of text-based terminal,  Commonly used terminals include Xterm, PuTTY, and a whole host of Telnet-enabled MUD clients including Tintin++, MushClient, and more.

# Requirements

Telnet++ requires a C++14 compiler and the Conan package manager.

# Features / Roadmap / Progress

1. [x] Basic constants and structures for use with Telnet
  * [x] Constants such as IAC, SB, SE, WILL, WONT, DO, DONT
  * [x] Helper structures for commands, negotiations, and subnegotiations
  * [x] Parser that can convert a stream of bytes into a variant of commands, negotiations, subnegotiations and plain text.
  * [x] Generator that can convert a stream of the above-mentioned variant into a stream of bytes.
2. [x] A framework for managing Telnet
  * [x] Structures for client and server options
  * [x] A method of routing incoming negotiations and subnegotiations to the relevant options.
3. [x] Reference implementations of various standard options
  * [x] Arbitrary "subnegotiationless" options
  * [x] Echo
  * [x] NAWS
  * [x] Terminal Type
  * [x] New Environ
4. [x] Reference implementations of some domain-specific options for MUDs
  * [x] MSDP - the Mud Server Data Protocol (see http://tintin.sourceforge.net/msdp/)
  * [x] MCCP - the Mud Client Compression Protocol (see http://tintin.sourceforge.net/mccp/)
5. [x] Structures to hide the complexity of the layer (e.g. routers, parsers, generators).
  * [x] Session class that understands all of the helper structures and how to convert to and from a stream of bytes.

# Status

Telnet++ is automatically tested with Clang and G++ 5.2.

# The Basics

The protocol has three basic elements, all of which are accessed by using the 0xFF character called "Interpret As Command", or IAC.

## Commands

Without needing to negotiate any capabilities, Telnet offers some out-of-the-box commands.  These include Are You There, which is usually sent by the client to provoke a response from an otherwise-busy server; Erase Line, which could be used in interative applications to cancel a set of input, and several other commands used for negotiations between options.

Commands are represented by the telnetpp::command class.

## Negotiations

The Telnet protocol describes a model whereby the client and server maintain separate lists of features, called "options", which can be enabled or disabled by the remote side.  Individual options may each be described as "server" or "client" options, and server and client options may be mixed on each side of the connection.  It is even possible in some cases that both sides of the connection can be both client and server for the same option.  These options are negotiated by using the commands DO, DONT, WILL and WONT.

The various Telnet option specifications are not consistent in what is considered a server and what is considered a client, but for the purposes of this library, the server is considered as the side of the connection that does the thing, and the client is the side of the connection that wants the thing.  That is, the server reacts to DO and DONT and sends WILL and WONT, and the client reacts to WILL and WONT and sends DO and DONT.

Negotiations are represented by the telnetpp::negotiation class.

## Subnegotiations

After an option has been negotiated, a new channel opens up to be able to communicate in an option-specific way to the remote terminal.  These are called subnegotiations, and each protocol defines its own sub-protocol.  For example, the NAWS (Negotiate About Window Size) sends five bytes when reporting window size, the first of which represents an "IS" token, and the following four bytes represent two two-byte pairs that are the window extends.

Subnegotiations are represented by the telnetpp::subnegotiation class.

# Dataflow: Elements, Tokens and Streams

A telnetpp::element is a Boost.Variant that may contain a command, a negotiation, a subnegotiation, or just a plain sequence of bytes representing non-Telnet-specific input/output.

# Stream-Unaware

The Telnet++ library does not impose any requirement on any kind of data stream API.  In order to accomplish this, it makes heavy use of continuation functions.  See the telnetpp::session class for an in-depth explanation of how this works.

# Options

As alluded to earlier, each distinct feature is represented by either a telnetpp::client_option or a telnetpp::server_option.  These both enjoy the same API; they only differ in the underlying protocol.  The user needs to know little about which actual negotiations and commands are sent.  There are two key functions and one signal for the option classes:

* activate() - this is used to request activation on the remote side.
* deactive() - this is used to request deactivation on the remote side.
* on_state_changed - this is a signal that is called when the option is either being activated, active, being deactivated, or has become inactive.

# Session

All of the above can be quite complicated to manage, so Telnet++ provides the telnetpp::session class.  This is the key abstraction of the Telnet++ library, and is used to manage an entire Telnet feature set for a connection.  This is accomplished by "install"ing handlers for commands and options:

```
// A user-specified function for sending bytes to the remote.
void my_socket_send(telnetpp::bytes data);

// Create a session object, which manages the inputs and outputs from my connection.  It requires
// a function that is to be called whenever non-Telnet input is received.
telnetpp::session session;

// An echo server (provided with Telnet++) is used to control whether a server responds to input from
// a client by transmitting the same text back to the client.  By default, this does not happen, and
// clients print out locally whatever is typed in.  By activating this option, the client no longer
// locally echos input, and the server is totally in control of what appears on the screen.
telnetpp::options::echo::server echo_server;

// The session now knows we want this feature to be handled and does all the heavy lifting for us.
session.install(echo_server);

// By default, options sit there in a deactivated state unless explicitly activated either locally
// in code or in protocol from the remote.  Here, we activate it ourselves, forwarding protocol
// via the session, and eventually out through our user-specified function.
echo_server.activate(
    [&](telnetpp::element const &elem)
    {
        session.send(elem, my_socket_send);
    });

// Sessions just pass on commands to functions installed on a per-command basis.  Here we pass a
// lambda to handle the Are You There command.
session.install(
    telnetpp::command{telnetpp::ayt},
    [&]
    {
        // We respond here by forwarding plain text as a sequence of bytes via the session.
        using telnetpp::literals;
        auto const message = "Yes, I'm here"_tb;
        
        session.send(message, my_socket_send);
    });
```

Receiving data is slightly more complex in that any reception of data may require data to be sent, and so functions that receive data also have a continuation for what to do with the response.

```
// A user-specified function used for receiving bytes sent from the
// remote.
int my_socket_receive(telnetpp::byte *buffer, int size);

// A user-specified function that transmits data up to the application
// Note that the second argument is used to tell the application how
// it may send a respond to the data it receives.
void my_application_receive(
    telnetpp::bytes data,
    std::function<void (telnetpp::bytes)> const &send);

telnetpp::byte my_buffer[1024];
int amount_received = my_socket_receive(my_buffer, 1024);

session.receive(
    telnetpp::bytes{my_buffer, amount_received},
    my_application_receive,
    my_socket_send);
```
