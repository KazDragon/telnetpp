#ifndef TELNETPP_SERVER_OPTION
#define TELNETPP_SERVER_OPTION

#include "telnetpp/core.hpp"
#include "telnetpp/token.hpp"
#include <boost/signals2.hpp>
#include <vector>

namespace telnetpp {

//* =========================================================================
/// \brief A class that represents a Telnet option's server side.  That is,
/// the side that received DO and DONT negotiations and sends WILL and WONT
/// negotiations.
/// 
/// \par
/// Note that the usage of server in this context may disagree with a
/// particular option's RFC specification.  The determination of what is a
/// server and what is a client is not rigorously applies throughout the
/// RFCs, so consider this merely an implementation detail of this library.
//* =========================================================================
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
    /// \brief Allows the option to be activated by the remote end.
    //* =====================================================================
    void set_activatable();

    //* =====================================================================
    /// \brief Activates the option.
    //* =====================================================================
    std::vector<telnetpp::token_pass> activate();
    
    //* =====================================================================
    /// \brief Deactivates the option.
    //* =====================================================================
    std::vector<telnetpp::token_pass> deactivate();

    //* =====================================================================
    /// \brief Returns true iff the option is active.
    //* =====================================================================
    bool is_active() const;

    //* =====================================================================
    /// \brief Makes a request of the option
    //* =====================================================================
    std::vector<telnetpp::token_pass> negotiate(telnetpp::u8 request);
    
    //* =====================================================================
    /// \brief Send a subnegotiation to the option.
    //* =====================================================================
    std::vector<telnetpp::token_pass> subnegotiate(
        std::vector<telnetpp::u8> const &content);

    boost::signals2::signal<
        std::vector<telnetpp::token_pass> (),
        token_combiner<telnetpp::token_pass>
    > on_state_changed;
    
private :
    //* =====================================================================
    /// \brief Handle a negotiation that has been received in the active
    /// state.
    //* =====================================================================
    virtual std::vector<telnetpp::token_pass> handle_subnegotiation(
        std::vector<telnetpp::u8> const &content);

    enum class state
    {
        inactive,
        activating,
        active,
        deactivating,
    };
    
    state state_       = state::inactive;
    bool  activatable_ = false;
    u8    option_;
};

class negotiation_router;
class subnegotiation_router;

//* =========================================================================
/// \brief Registers an option with negotiation and subnegotiation routers.
//* =========================================================================
void register_server_option(
    server_option &option,
    negotiation_router &neg_router,
    subnegotiation_router &sub_router);

}

#endif
