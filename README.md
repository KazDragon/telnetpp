# Telnet++

[![Documentation](https://codedocs.xyz/KazDragon/telnetpp.svg)](https://codedocs.xyz/KazDragon/telnetpp/)
[![License](https://img.shields.io/github/license/KazDragon/telnetpp.svg)](https://en.wikipedia.org/wiki/MIT_License)
[![GitHub Releases](https://img.shields.io/github/release/KazDragon/telnetpp.svg)](https://github.com/KazDragon/telnetpp/releases)
[![Build Status](https://travis-ci.org/KazDragon/telnetpp.svg?branch=master)](https://travis-ci.org/KazDragon/telnetpp)
[![Coverage Status](https://coveralls.io/repos/github/KazDragon/telnetpp/badge.svg?branch=master)](https://coveralls.io/github/KazDragon/telnetpp?branch=master)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/63ec54845f2c41f1899706c61f1c316b)](https://www.codacy.com/app/KazDragon/telnetpp?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=KazDragon/telnetpp&amp;utm_campaign=Badge_Grade)

[![Github Issues](https://img.shields.io/github/issues/KazDragon/telnetpp.svg)](https://github.com/KazDragon/telnetpp/issues)
[![Stories in Ready](https://badge.waffle.io/KazDragon/telnetpp.png?label=ready&title=Stories%20In%20Ready)](https://waffle.io/KazDragon/telnetpp)
[![Stories in Progress](https://badge.waffle.io/KazDragon/telnetpp.png?label=in%20progress&title=Stories%20In%20Progress)](https://waffle.io/KazDragon/telnetpp)

[![Gitter](https://badges.gitter.im/KazDragon/telnetpp.svg)](https://gitter.im/KazDragon/telnetpp?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)
[![This is a forkable respository](https://img.shields.io/badge/forkable-yes-brightgreen.svg)](https://basicallydan.github.io/forkability/?u=KazDragon&r=telnetpp)

Telnet++ is an implementation of the Telnet Session Layer protocol that is used primarily to negotiate a feature set between a client and server, the former of which is usually some kind of text-based terminal,  Commonly used terminals include Xterm, PuTTY, and a whole host of Telnet-enabled MUD clients including Tintin++, MushClient, and more.

# Requirements

Telnet++ requires a C++14 compiler and the Boost Libraries.  It also uses Google Test for its testing suite, which is compiled optionally.

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
  * [x] A byte_converter, which converts a stream of tokens into streams of bytes.

# Status

Telnet++ is automatically tested with Clang and G++ 5.2.

For further information about the working status of Telnet++, to report any bugs, or to make any feature requests, visit the [Waffle board](https://waffle.io/KazDragon/telnetpp)

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

A telnetpp::element is a Boost.Variant that may contain a command, a negotiation, a subnegotiation, or just a plain text string representing non-Telnet-specific input/output.

Furthermore, it is also possible to send objects of any other type using Boost.Any.  The main structure used to pass information around the library is therefore the telnetpp::token, which is a Boost.Variant that may contain either a Boost.Any or a telnetpp::element.

When communicating with the lower byte-streaming layer, telnetpp::elements are transformed into byte streams, which are represented by the u8stream type.

After a telnetpp::token passes through the Telnet layer, this results in a telnetpp::stream_token, which is a Boost.Variant of a telnetpp::u8stream and a Boost.Any.  This enables layers above Telnet to talk to layers below Telnet.

Finally, when the telnetpp::stream_token has passed through any lower layers, any Boost.Any instances remaining can be filtered out and converted into a telnetpp::u8stream, which can be sent across the data channel proper.

# Stream-Unaware

The Telnet++ library does not impose any requirement on any kind of data stream API.  In order to accomplish this, most parts of the API do not actually send any data; rather, they return the data to be sent, usually in the form of either collections of tokens, or as streams (described above).

# Options

As alluded to earlier, each distinct feature is represented by either a telnetpp::client_option or a telnetpp::server_option.  These both enjoy the same API; they only differ in the underlying protocol.  The user needs to know little about which actual negotiations and commands are sent.  There are two key functions and one signal for the option classes:

* activate() - this is used to request activation on the remote side.
* set_activatable() - this is used to allow the option to be activated from the remote side.
* on_state_changed - this is a signal that is called when the option is either being activated, active, being deactivated, or has become inactive.

# Session

All of the above can be quite complicated to manage, so Telnet++ provides the telnetpp::session class.  This can be used to manage an entire Telnet feature set for a connection.  This is accomplished by simply "install"ing handlers for commands and options:

```
// Create a session object, which manages the inputs and outputs from my connection.  It requires
// a function that is to be called whenever non-Telnet input is received.
telnetpp::session session(
    [](std::string const &text)
      -> std::vector<telnetpp::token>
    {
        return app_input_received(text);
    });

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
// or something wrapped in a boost::any.  For most usages, using telnetpp::byte_converter to
// filters the boost::any out would suffice.  But see the appendix on MCCP below for why the
// API exists in this way.
void my_lower_layer_send(vector<telnetpp::stream_token> const &);

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
// echo_server.activate() returns the tokens necessary to begin the activation.
// session.send() turns the elements in the tokens into bytes.
// my_lower_layer_send() sends the streams of bytes to the connection.
my_lower_layer_send(session.send(echo_server.activate()));
```

# Epilogue

The code above: the session set-up, and the send/receive functions, are all that's necessary to use the Telnet++ library.  Further sample usage can be found in Paradice9 (see https://github.com/KazDragon/paradice9/blob/master/paradice/connection.cpp -- in particular, just search for the lines containing the telnet_session_ variable.)

# Appendix: MCCP

The Mud Client Compression Protocol v2 (http://tintin.sourceforge.net/mccp/) is a unique protocol in that activating it also changes the way the Telnet Protocol is carried, since it too is compressed.  This requires careful design in order to ensure that uncompressed data is not sent or received when compressed data has been sent or received, and vice versa.

The following is a rough guide to how to integrate the MCCP functionality.  How it actually integrates with your application will vary.  Consider functions starting with "app" to be something in your application above the Telnet layer that receives user input, and those starting with "my" to be an interaction between the Telnet layer and a lower layer that deals with sending and receiving byte streams, such as a socket API.

```
// class variables for the connection
telnetpp::session session(
    [](std::string const &text) -> std::vector<telnetpp::token>
    {
        // forward to my application
        return app_input_received(text);
    });
    
telnetpp::options::mccp::server mccp_server;
    
telnetpp::options::mccp::codec mccp_codec(
    std::make_shared<telnetpp::options::mccp::zlib_compressor>(),
    std::make_shared<telnetpp::options::mccp::zlib_decompressor>());

telnetpp::byte_converter byte_coverter;

// .. in initialization function
session.install(mccp_server);

// SKIP: activation of mccp_server.

// When receiving data from the lower layer, only bytes are expected, so the additional baggage
// of the boost::any does not have to be handled in this direction.  However, do note that
// some inputs (e.g. option negotiations) have immediate outputs.  Therefore, it's necessary
// to send the response from any data received.
void my_layer_receive(telnetpp::u8stream const &stream)
{
    my_lower_layer_send(session.send(session.receive(stream)));
}

// This function is called with the results of session.send(), meaning that any telnet
// elements have been converted to bytes.  It has two responsibilities: it must first
// compress (if necessary) any bytes in the data, and then it must send the data on
// to the raw connection.
void my_lower_layer_send(vector<telnetpp::stream_token> const &tokens)
{
    // NOTE: this consumes some of the Boost.Any elements present at this layer of
    // the protocol.
    vector<telnetpp::stream_token> const &compressed_tokens = codec.send(tokens);
    
    // NOTE: it can be assumed that no Boost.Any elements are present and, if they
    // are present, they are not used.  Therefore, we can just filter them out and
    // return the entire set as one stream of bytes.
    telnetpp::u8stream const &stream = byte_converter.send(compressed_tokens);
    
    // Write the data according to whatever API is being used at the connection level.
    my_connection.write(stream.begin(), stream.end());
}

```

# Appendix: Testing

Unit testing is implemented with Google Test, but this requires a separate compilation.  The easiest way I've found of doing this is roughly equivalent to this script (which is mirrored in .travis.yml):

    sudo apt-get install libgtest-dev
    mkdir gtest
    cd gtest
    cmake /usr/src/gtest && make
    export GTEST_ROOT=$PWD

After this, re-running cmake with Telnet++ should pick up both the Google Test includes in /usr/include and also the binary which was just built.  If GTest was not found, Telnet++ will still build; just the automatic tests will be elided.
