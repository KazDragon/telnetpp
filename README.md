[![Stories on the Backlog](https://badge.waffle.io/KazDragon/telnetpp.png?label=backlog&title=Stories%20On%20The%20Backlog)](https://waffle.io/KazDragon/telnetpp)

[![Stories in Ready](https://badge.waffle.io/KazDragon/telnetpp.png?label=ready&title=Stories%20In%20Ready)](https://waffle.io/KazDragon/telnetpp)

[![Stories in Progress](https://badge.waffle.io/KazDragon/telnetpp.png?label=in%20progress&title=Stories%20In%20Progress)](https://waffle.io/KazDragon/telnetpp)


# Telnet++
Telnet++ is an implementation of the Telnet Session Layer protocol that is used primarily to negotiate a feature set between a client and server, the former of which is usually some kind of text-based terminal,  Commonly used terminals include Xterm, PuTTY, and a whole host of Telnet-enabled MUD clients including Tintin++, MushClient, and more.

# Requirements

Telnet++ requires a C++14 compiler and the Boost Libraries.  It also uses CppUnit for its testing suite, which is compiled optionally.

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
4. [x] Structures to hide the complexity of the layer (e.g. routers, parsers, generators).
  * [x] Session class that understands all of the helper structures and how to convert to and from a stream of bytes.

# Status
[![Build Status](https://travis-ci.org/KazDragon/telnetpp.svg?branch=master)](https://travis-ci.org/KazDragon/telnetpp)
Telnet++ is automatically tested with Clang and G++ 5.2.

For further information about the working status of Telnet++, to report any bugs, or to make any feature requests, visit the [Waffle board](https://waffle.io/KazDragon/telnetpp)

# The Basics

The protocol has three basic elements, all of which are accessed by using the 0xFF character called "Interpret As Command", or IAC.

## Commands

Without needing to negotiate any capabilities, Telnet offers some out-of-the-box commands.  These include Are You There, which is usually sent by the client to provoke a response from an otherwise-busy server; Erase Line, which could be used in interative applications to cancel a set of input, and several other commands used for negotiations between options.

Commands are represented by the telnetpp::command class.

## Negotiations

The Telnet protocol describes a model whereby the client and server maintain separate lists of features, called "options", which can be enabled or disabled by the remote side.  Individual options may each be described as "server" or "client" options, and server and client options may be mixed on each side of the connection.  It is even possible in some cases that both sides of the connection can be both client and server for the same option.  These options are negotiated by using the commands DO, DONT, WILL and WONT, and although the option specifications are not consistent in the usage, for the purposes of this library, the server is considered as the side of the connection that does the thing, and the client is the side of the connection that wants the thing.  That is, the server reacts to DO and DONT and sends WILL and WONT, and the client reacts to WILL and WONT and sends DO and DONT.

Negotiations are represented by the telnetpp::negotiation class.

## Subnegotiations

After an option has been negotiated, a new channel opens up to be able to communicate in an option-specific way to the remote terminal.  These are called subnegotiations, and each protocol defines its own sub-protocol.  For example, the NAWS (Negotiate About Window Size) sends five bytes when reporting window size, the first of which represents an "IS" token, and the following four bytes represent two two-byte pairs that are the window extends.

Subnegotiations are represented by the telnetpp::subnegotiation class.

# Elements, Tokens and Streams

A telnetpp::element is a Boost.Variant that may contain a command, a negotiation, a subnegotiation, or just a plain text string representing non-Telnet-specific input/output.

Furthermore, in order to enable certain forms of API (see the appendix on MCCP later), it is also possible to send objects of any other type using Boost.Any.  The main structure used to pass information around the library is therefore the telnetpp::token, which is a Boost.Variant that may contain either a Boost.Any or a telnetpp::element.

When communicating with the lower byte-streaming layer, telnetpp::elements are transformed into byte streams, which are represented by the u8stream type.

# Stream-Unaware

The Telnet++ library does not impose any requirement on any kind of data stream API.  In order to accomplish this, most parts of the API do not actually send any data; rather, they return the data to be sent, usually in the form of either collections of tokens, or as streams (described above).

# Options

As alluded to earlier, each distinct feature is represented by either a telnetpp::client_option or a telnetpp::server_option.  These both enjoy the same API; they only differ in the underlying protocol.  The user needs to know little about which actual negotiations and commands are sent.  There are two key functions and one signal for the option classes:

* activate() - this is used to request activation on the remote side.
* set_activatable() - this is used to allow the option to be activated from the remote side.
* on_state_changed - this is a signal that is called when the option becomes either enabled or disabled.

# Session

All of the above can be quite complicated to manage, so Telnet++ provides the telnetpp::session class.  This can be used to manage an entire Telnet feature set for a connection.  This is accomplished by simply "install"ing handlers for commands and options:

```
telnetpp::session session;

// An echo server (provided with Telnet++) is used to control whether a server responds to input from
// a client by transmitting the same text back to the client.  By default, this does not happen, and
// clients print out locally whatever is typed in.  By activating this option, the client no longer
// locally echos input, and the server is totally in control of what appears on the screen.
telnetpp::options::echo::server echo_server;

// This allows the client to tell the server to perform the echo.
echo_server.set_activatable();

// The session now knows we want this feature to be handled and does all the heavy lifting for us.
session.install(echo_server);

// Sessions just pass on commands to functions installed on a per-command basis.  Here we pass a
// lambda to handle the Are You There command.
session.install(telnetpp::command(telnetpp::ayt)),
    [](telnetpp::command const &cmd) -> std::vector<telnetpp::token>
    {
        // Handle the Are You There command.
        return {}; // Or whatever tokens you want...
    });
```

After this is done, the session can be used to both receive data from lower layers and to send information from higher layers.  Incoming data received in bytes is arranged into a u8stream.  This can be passed into the session's receive() function.  This converts the bytes into Telnet tokens, and these are routed appropriately to whichever settings are installed.

The result of this operation is a collection of tokens that represent the responses from the settings.  As mentioned above, these tokens may also include user-defined datastructures, as long as they are wrapped in a boost::any.  This can be used to communicate from layers above the Telnet session to the layers below.

The response of the session's receive() function should be sent immediately to the session's send() function.  This function transforms the Telnet structures in the response into a byte stream.  Any boost::any objects remain as they are unchanged.  This can then be passed onto the lower layer.

```
// When sending data to the lower layer, each portion of data may be either a stream of bytes,
// or something wrapped in a boost::any.  For most usages, a simple boost::static_visitor that
// filters the boost::any out would suffice.  But see the appendix on MCCP below for why this
// API exists.
void my_lower_layer_send(vector<variant<telnetpp::u8stream, any>> const &);

// When receiving data from the lower layer, only bytes are expected, so the additional baggage
// of the boost::any does not have to be handled in this direction.  However, do note that
// some inputs (e.g. option negotiations) have immediate outputs.  Therefore, it's necessary
// to send the response from any data received.
void my_layer_receive(telnetpp::u8stream const &stream)
{
    my_lower_layer_send(session.receive(stream));
}
```

Finally, a session's send() function can also be used to communicate directly with options.  For example,

```
my_lower_layer_send(session.send(echo_server.activate()));
```

# Epilogue

The code above: the session set-up, and the send/receive functions, are all that's necessary to use the Telnet++ library.  Further sample usage can be found in Paradice9 (see https://github.com/KazDragon/paradice9/blob/master/paradice/connection.cpp -- in particular, just search for the lines containing the telnet_session_ variable.)

# Appendix: MCCP

The Mud Client Compression Protocol v2 (http://tintin.sourceforge.net/mccp/) is a unique protocol in that activating it also changes the way the Telnet Protocol is carried, since it too is compressed.  This requires precise timing in order to work.  If the MCCP server has sent WILL MCCP, then it must not compress immediately, since it may be declined by a DONT MCCP response.  However, when the server then receives DO MCCP, it must compress from the very next output byte.  Another part of this is that, after either the server or client have sent a negotiation, it must not send any non-Telnet-specific data until the response is heard.  Otherwise, the remote connection may hear compressed/uncompressed data when it expects compressed/uncompressed data respectively.

Without hooking into a datastream API in any way, it is next to impossible to guarantee the timing of this data flow.  However, by embedding objects in the datastream representing these state changes, it becomes possible for precisely placed timing marks to be sent from the MCCP option implementation and received by a compression layer that is sitting below the session.
