[![Stories on the Backlog](https://badge.waffle.io/KazDragon/telnetpp.png?label=backlog&title=Stories%20On%20The%20Backlog)](https://waffle.io/KazDragon/telnetpp)

[![Stories in Ready](https://badge.waffle.io/KazDragon/telnetpp.png?label=ready&title=Stories%20In%20Ready)](https://waffle.io/KazDragon/telnetpp)

[![Stories in Progress](https://badge.waffle.io/KazDragon/telnetpp.png?label=in%20progress&title=Stories%20In%20Progress)](https://waffle.io/KazDragon/telnetpp)


# Telnet++
A C++ library that impelements a Telnet session application layer.

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
Telnet++ is automatically tested on:
* Clang 3.4 [![Build Status](https://travis-ci.org/KazDragon/telnetpp.svg?branch=master)](https://travis-ci.org/KazDragon/telnetpp)

For further information, visit the [Waffle board](https://waffle.io/KazDragon/telnetpp)

# About

Telnet is a Session Layer protocol that is used primarily to negotiate a feature set between a client and server, the former of which is usually some kind of text-based terminal,  Commonly used terminals include Xterm, PuTTY, and a whole host of Telnet-enabled MUD clients .  The protocol has three basic elements, all of which are accessed by using the 0xFF character called "Interpret As Command", or IAC.

## Commands

Without needing to negotiate any capabilities, Telnet offers some out-of-the-box commands.  These include Are You There, which is usually sent by the client to provoke a response from an otherwise-busy server; Erase Line, which could be used in interative applications to cancel a set of input, and several other commands used for negotiations between options.

Commands are represented by the telnetpp::command class.

## Negotiations

The Telnet protocol describes a model whereby the client and server maintain separate lists of features, called "options", which can be enabled or disabled by the remote side.  Individual options may each be described as "server" or "client" options, and server and client options may be mixed on each side of the connection.  It is even possible in some cases that both sides of the connection can be both client and server for the same option.  These options are negotiated by using the commands DO, DONT, WILL and WONT, and although the option specifications are not consistent in the usage, for the purposes of this library, the server is considered as the side of the connection that does the thing, and the client is the side of the connection that wants the thing.  That is, the server reacts to DO and DONT and sends WILL and WONT, and the client reacts to WILL and WONT and sends DO and DONT.

Negotiations are represented by the telnetpp::negotiation class.

## Subnegotiations

After an option has been negotiated, a new channel opens up to be able to communicate in an option-specific way to the remote terminal.  These are called subnegotiations, and each protocol defines its own sub-protocol.  For example, the NAWS (Negotiate About Window Size) sends five bytes when reporting window size, the first of which represents an "IS" token, and the following four bytes represent two two-byte pairs that are the window extends.

# Architecture

Telnet++ is a completely datasource-agnostic.  It doesn't know whether you're attached to a network socket, a local pipe, reading a file, an in-memory stream or whatever.  And it doesn't care.  For that reason, functions that require data to be transmitted to the remote end of the datastream merely return the data that needs to be sent.  This includes functions that in principle receive data from the datastream and need to transmit something in response.  For that reason, it's wise to construct callbacks within your own code that talks with Telnet++ in the say way.

# Read / Write Loop

Telnet++ is designed to be called in a read-write loop that follows this basic pattern:

## Session Set-Up / Initialization

A Telnet Session object is created, and handlers are installed for the various plug-ins that support the various client and server options, and handlers for commands are installed:

```
telnetpp::session session;
telnetpp::options::echo::server echo_server;

session.install(echo_server);
session.install(telnetpp::command(telnetpp::ayt)),
    [&result](telnetpp::command const &cmd) -> std::vector<telnetpp::token>
    {
        // Handle the Are You There command.
        return {}; // Or whatever tokens you want...
    });
```

## Session Use

Incoming data received in bytes is arranged into a u8stream (which is some type that models the standard Container concept.  This can be passed into the session's receive() function.  This converts the bytes into Telnet tokens (commands, negotiations, subnegotiations), and these are routed appropriately to whichever settings are installed.

The result of this operation is a sequence of tokens that represent the responses from the settings.  These tokens may also include user-defined datastructures, as long as they are wrapped in a boost::any.  This can be used to communicate from layers above the Telnet session to the layers below.

The response of the session's receive() function should be sent immediately to the session's send() function.  This function transforms the Telnet structures in the response into a byte stream.  Any boost::any objects remain as they are unchanged.  This can then be passed onto the lower layer.

```
void my_lower_layer_send(vector<variant<telnetpp::u8stream, any>> const &);

void my_layer_receive(telnetpp::u8stream const &stream)
{
    my_lower_layer_send(session.receive(stream));
}

```

A session's send() function can also be used to communicate directly with options.  For example,

```
session.send(echo_server.activate());
```
