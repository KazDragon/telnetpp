[![Stories on the Backlog](https://badge.waffle.io/KazDragon/telnetpp.png?label=backlog&title=Stories%20On%20The%20Backlog)](https://waffle.io/KazDragon/telnetpp)

[![Stories in Ready](https://badge.waffle.io/KazDragon/telnetpp.png?label=ready&title=Stories%20In%20Ready)](https://waffle.io/KazDragon/telnetpp)

[![Stories in Progress](https://badge.waffle.io/KazDragon/telnetpp.png?label=in%20progress&title=Stories%20In%20Progress)](https://waffle.io/KazDragon/telnetpp)


# Telnet++
A C++ library for interacting with TELNET streams

# Features / Roadmap / Progress

1. [ ] Basic constants and structures for use with TELNET
  * [x] Constants such as IAC, SB, SE, WILL, WONT, DO, DONT
  * [x] Helper structures for commands, negotiations, and subnegotiations
  * [x] Parser that can convert a stream of bytes into a variant of commands, negotiations, subnegotiations and plain text.
  * [x] Generator that can convert a stream of the above-mentioned variant into a stream of bytes.
2. [ ] A framework for managing TELNET
  * [ ] Structures for client and server options
  * [ ] A method of routing incoming negotiations and subnegotiations to the relevant options.
3. [ ] Reference implementations of various standard options
  * [ ] Arbitrary "subnegotiationless" options
  * [ ] Echo
  * [ ] NAWS
  * [ ] Terminal Type

# Status
Telnet++ is automatically tested on:
* Clang 3.4 [![Build Status](https://travis-ci.org/KazDragon/telnetpp.svg?branch=master)](https://travis-ci.org/KazDragon/telnetpp)

For further information, visit the [Waffle board](https://waffle.io/KazDragon/telnetpp)
