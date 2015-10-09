#ifndef TELNETPP_OPTIONS_NAWS_SERVER
#define TELNETPP_OPTIONS_NAWS_SERVER

#include "telnetpp/server_option.hpp"

namespace telnetpp { namespace options { namespace naws {

//* =========================================================================
/// \brief An implementation of the server side of the Telnet NAWS option.
//* =========================================================================
class server : public telnetpp::server_option {
public :
    //* =====================================================================
    /// CONSTRUCTOR
    //* =====================================================================
    server();

    boost::signals2::signal<
        std::vector<telnetpp::token> (telnetpp::u16, telnetpp::u16),
        telnetpp::token_combiner
    > on_window_size_changed;

private :
    //* =====================================================================
    /// \brief Handle a negotiation that has been received in the active
    /// state.
    //* =====================================================================
    std::vector<telnetpp::token> handle_subnegotiation(
        std::vector<telnetpp::u8> const &content) override;

};

}}}

#endif
