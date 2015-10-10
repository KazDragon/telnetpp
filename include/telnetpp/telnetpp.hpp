#ifndef TELNETPP_TELNETPP_HPP_
#define TELNETPP_TELNETPP_HPP_

// routing_visitor pulls in all of the various routers, and thus all
// of the various token types.
#include "telnetpp/routing_visitor.hpp"

// With the addition of the parser and generator, the client need then only
// include the specific required options in order to have a complete Telnet
// layer.
#include "telnetpp/parser.hpp"
#include "telnetpp/generator.hpp"

#endif
