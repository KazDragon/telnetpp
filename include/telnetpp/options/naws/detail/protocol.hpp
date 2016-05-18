#pragma once

#include "telnetpp/core.hpp"

//* =========================================================================
/// \namespace telnetpp::options::naws
/// \brief An implementation of the standard Telnet Negotiate About Window
/// Size option.
/// \par Overview
/// The NAWS option is used for a server to inform a client about the extents
/// if its window size/text area.
/// \par Usage
/// Construct an option as normal and install it into a session.  If you are
/// a client, register for callbacks with the on_window_size_changed signal.
/// If you are a server, send updates about your window size using the
/// set_window_size() function.
/// \see https://www.ietf.org/rfc/rfc1073.txt
//* =========================================================================
namespace telnetpp { namespace options { namespace naws { namespace detail {

static constexpr telnetpp::u8 option = 31;

}}}}
