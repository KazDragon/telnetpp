#ifndef TELNETPP_OPTIONS_NAWS_SERVER
#define TELNETPP_OPTIONS_NAWS_SERVER

#include "telnetpp/server_option.hpp"

namespace telnetpp { namespace options { namespace naws {

class server : public telnetpp::server_option {
public :
    //* =====================================================================
    /// CONSTRUCTOR
    //* =====================================================================
    server();

    boost::signals2::signal<void (telnetpp::u16, telnetpp::u16)> on_window_size_changed;

private :
    //* =====================================================================
    /// \brief Handle a negotiation that has been received in the active
    /// state.
    //* =====================================================================
    void handle_subnegotiation(
        std::vector<telnetpp::u8> const &content) override;

};

}}}

#endif
