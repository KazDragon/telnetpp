#pragma once

// For sending commands, it's necessary to know what the commands in the
// protocol are.
#include "telnetpp/protocol.hpp"

// With the addition of the session object, the client need then only
// include the specific required options in order to have a complete Telnet
// layer.
#include "telnetpp/session.hpp"
