#pragma once

#include "telnetpp/client_option.hpp"

namespace telnetpp { namespace options { namespace terminal_type {

//* =========================================================================
/// \brief An implementation of the client side of the Telnet Terminal
/// Type option.
//* =========================================================================
class TELNETPP_EXPORT client : public telnetpp::client_option {
public :
    //* =====================================================================
    /// CONSTRUCTOR
    //* =====================================================================
    client();

    //* =====================================================================
    /// \brief Requests that the remote end send its terminal type.
    //* =====================================================================
    std::vector<telnetpp::token> request_terminal_type();

    boost::signals2::signal
    <
        std::vector<telnetpp::token> (std::string const &),
        telnetpp::token_combiner
    > on_terminal_type;

private :
    //* =====================================================================
    /// \brief Handle a negotiation that has been received in the active
    /// state.
    //* =====================================================================
    std::vector<telnetpp::token> handle_subnegotiation(
        byte_stream const &content) override;
};

}}}
