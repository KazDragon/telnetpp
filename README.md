[![Stories on the Backlog](https://badge.waffle.io/KazDragon/telnetpp.png?label=backlog&title=Stories%20On%20The%20Backlog)](https://waffle.io/KazDragon/telnetpp)

[![Stories in Ready](https://badge.waffle.io/KazDragon/telnetpp.png?label=ready&title=Stories%20In%20Ready)](https://waffle.io/KazDragon/telnetpp)

[![Stories in Progress](https://badge.waffle.io/KazDragon/telnetpp.png?label=in%20progress&title=Stories%20In%20Progress)](https://waffle.io/KazDragon/telnetpp)


# Telnet++
A C++ library for interacting with TELNET streams

# Features / Roadmap / Progress

1. [x] Basic constants and structures for use with TELNET
  * [x] Constants such as IAC, SB, SE, WILL, WONT, DO, DONT
  * [x] Helper structures for commands, negotiations, and subnegotiations
  * [x] Parser that can convert a stream of bytes into a variant of commands, negotiations, subnegotiations and plain text.
  * [x] Generator that can convert a stream of the above-mentioned variant into a stream of bytes.
2. [x] A framework for managing TELNET
  * [x] Structures for client and server options
  * [x] A method of routing incoming negotiations and subnegotiations to the relevant options.
3. [ ] Reference implementations of various standard options
  * [x] Arbitrary "subnegotiationless" options
  * [x] Echo
  * [x] NAWS
  * [ ] Terminal Type

# Status
Telnet++ is automatically tested on:
* Clang 3.4 [![Build Status](https://travis-ci.org/KazDragon/telnetpp.svg?branch=master)](https://travis-ci.org/KazDragon/telnetpp)

For further information, visit the [Waffle board](https://waffle.io/KazDragon/telnetpp)

# Architecture

Telnet++ is a completely datasource-agnostic.  It doesn't know whether you're attached to a network socket, a local pipe, reading a file, an in-memory stream or whatever.  And it doesn't care.  For that reason, functions that require data to be transmitted to the remote end of the datastream merely return the data that needs to be sent.  This includes functions that in principle receive data from the datastream and need to transmit something in response.  For that reason, it's wise to construct callbacks within your own code that talks with Telnet++ in the say way.

# Read / Write Loop

Telnet++ is designed to be called in a read-write loop that follows this basic pattern:

## Parsing

Incoming data received in bytes is arranged into a std::vector<telnetpp::u8>.  telnetpp::parse() is then used to translate this into a std::vector<telnetpp::token>.  A token is a variant of std::string, to represent data that is not parsed into any special structure and can be passed onto a higher layer; telnetpp::command; telnetpp::negotiation; and telnetpp::subnegotiation.

```
std::vector<telnetpp::u8> unparsed_bytes;

// ...

auto bytes_in = datastream.read(); // for example
unparsed_bytes.insert(unparsed_bytes.end(), bytes_in.begin(), bytes_in.end());

auto begin = unparsed_bytes.begin();
auto end = unparsed_bytes.end();

auto tokens = telnetpp::parse(begin, end);
    // Note: begin is modified here so that [begin, end) is the unparsed data.
unparsed_bytes.erase(unparsed_bytes.begin(), begin);
    // and so the parsed bytes can be discarded.
    
// We now have a vector of tokens to use somehow.
```

## Routing

Telnet++ comes with a set of "routers" for each Telnet++ datatype.  These can be used to automatically route tokens to registered targets.  A routing_visitor is supplied to aid in this process:

```
std::function<std::vector<telnetpp::token> (std::string const &)> on_text = // ...
    // on_text is set to point to your higher layer.  Don't forget to return any tokens
    // resulting from your higher layer's call to the telnet layer.
telnetpp::command_router command_router;
telnetpp::negotiation_router negotiation_router;
telnetpp::subnegotiation_router subnegotiation_router;

// Various targets register with the routers, including text and command handlers,
// and server and client options.  For example...
telnetpp::options::naws::server naws_server;
naws_server.on_window_size_changed.connect(
    [](telnetpp::u16 width, telnetpp::u16 height) -> std::vector<telnetpp::token>
    {
        // call my function that knows about screen size changes, return
        // whatever result is appropriate.
    });
    
// This function registers the naws_server's desired messages with the routers above.
telnetpp::register_server_option(naws_server, negotiation_router, subnegotiation_router);

// Create a visitor that can take streams of incoming tokens and ship them off to the
// correct routers.
telnetpp::routing_visitor visitor(
    on_text,
    command_router,
    negotiation_router,
    subnegotiation_router);

// ... somewhere, the parsing of incoming bytes happened and we have the vector of tokens.

std::vector<telnetpp::token> responses;

for (auto &&tok : tokens)
{
    // Ship each token off to the correct router for whatever the token was.
    auto response = boost::apply visitor(visitor, tok);
    
    // Add the response to the container of responses.
    responses.insert(responses.end(), response.begin(), response.end());
}
```

## Generation

Finally, we have a container of responses that have been accumulated from the operations called by the incoming data.  These can be translated into a std::vector<telnetpp::u8> by using telnetpp::generate.  This can then be sent to the datastream.

```
auto bytes_out = telnetpp::generate(responses);
datastream.write(bytes_out); // for example
```
