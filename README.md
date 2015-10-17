[![Stories on the Backlog](https://badge.waffle.io/KazDragon/telnetpp.png?label=backlog&title=Stories%20On%20The%20Backlog)](https://waffle.io/KazDragon/telnetpp)

[![Stories in Ready](https://badge.waffle.io/KazDragon/telnetpp.png?label=ready&title=Stories%20In%20Ready)](https://waffle.io/KazDragon/telnetpp)

[![Stories in Progress](https://badge.waffle.io/KazDragon/telnetpp.png?label=in%20progress&title=Stories%20In%20Progress)](https://waffle.io/KazDragon/telnetpp)


# Telnet++
A C++ library for interacting with Telnet streams

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
