#pragma once

#include "telnetpp/client_option.hpp"

namespace telnetpp { namespace options { namespace naws {

//* =========================================================================
/// \brief An implementation of the client side of the Telnet NAWS option.
//* =========================================================================
class TELNETPP_EXPORT client : public telnetpp::client_option {
public :
    using window_dimension = std::uint16_t;

    //* =====================================================================
    /// CONSTRUCTOR
    //* =====================================================================
    client();

    boost::signals2::signal<
        std::vector<telnetpp::token> (window_dimension, window_dimension),
        telnetpp::token_combiner
    > on_window_size_changed;

private :
    //* =====================================================================
    /// \brief Handle a negotiation that has been received in the active
    /// state.
    //* =====================================================================
    std::vector<telnetpp::token> handle_subnegotiation(
        byte_stream const &content) override;

};

}}}
