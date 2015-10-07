#ifndef TELNETPP_SERVER_OPTION
#define TELNETPP_SERVER_OPTION

#include "telnetpp/core.hpp"
#include "telnetpp/token.hpp"
#include <boost/signals2.hpp>
#include <vector>

namespace telnetpp {

class server_option {
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    server_option(telnetpp::u8 option);
    
    //* =====================================================================
    /// \brief Returns the option code.
    //* =====================================================================
    telnetpp::u8 option() const;
    
    //* =====================================================================
    /// \brief Activates the option.
    //* =====================================================================
    std::vector<telnetpp::token> activate();
    
    //* =====================================================================
    /// \brief Deactivates the option.
    //* =====================================================================
    std::vector<telnetpp::token> deactivate();

    //* =====================================================================
    /// \brief Returns true iff the option is active.
    //* =====================================================================
    bool is_active() const;

    //* =====================================================================
    /// \brief Makes a request of the option
    //* =====================================================================
    std::vector<telnetpp::token> negotiate(telnetpp::u8 request);
    
    //* =====================================================================
    /// \brief Send a subnegotiation to the option.
    //* =====================================================================
    std::vector<telnetpp::token> subnegotiate(
        std::vector<telnetpp::u8> const &content);

    boost::signals2::signal<
        std::vector<token> (),
        token_combiner
    > on_state_changed;
    
private :
    //* =====================================================================
    /// \brief Handle a negotiation that has been received in the active
    /// state.
    //* =====================================================================
    virtual std::vector<telnetpp::token> handle_subnegotiation(
        std::vector<telnetpp::u8> const &content);

    enum class state
    {
        inactive,
        activating,
        active,
        deactivating,
    };
    
    state state_ = state::inactive;
    u8 option_;
};

}

#endif
