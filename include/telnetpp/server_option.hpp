#pragma once

#include "telnetpp/detail/option.hpp"

namespace telnetpp {

//* =========================================================================
/// \brief A class that represents a Telnet option's server side.  That is,
/// the side that receives DO and DONT negotiations and sends WILL and WONT
/// negotiations.
///
/// \par Server vs. Client
/// Note that the usage of server in this context may disagree with a
/// particular option's RFC specification.  The determination of what is a
/// client and what is a server is not rigorously applies throughout the
/// RFCs, so consider this merely an implementation detail of this library.
/// \par Usage
/// server_option provides a general interface to enable the implementation
/// of the serverlient sides of all Telnet options. There are two customization
/// points that must be filled in before an implementation is complete:
/// \li the derived class must call the constructor, passing in a value
/// that is the Option Code for the feature being implemented.
/// \li the derived class must implement the handle_subnegotiation function.
/// This is called whenever data specific to the option passes through the
/// Telnet layer.
///
/// A final third customization point is the on_state_changed signal, to which
/// a user can connect to have functionality executed when the option changes
/// state.
/// \see https://tools.ietf.org/html/std8
//* =========================================================================
class TELNETPP_EXPORT server_option
  : public telnetpp::detail::option<
        telnetpp::will,
        telnetpp::wont,
        telnetpp::do_,
        telnetpp::dont
    >
{
protected:
    //* =====================================================================
    /// \brief Constructor
    /// \param code The option code for this option.
    //* =====================================================================
    explicit server_option(option_type code);
};

using server_options = gsl::span<server_option>;

}
