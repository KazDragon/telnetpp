#ifndef TELNETPP_SERVER_OPTION
#define TELNETPP_SERVER_OPTION

#include "telnetpp/element.hpp"
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
class TELNETPP_EXPORT server_option {
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
        u8stream const &content);

    boost::signals2::signal<
        std::vector<telnetpp::token> (),
        token_combiner
    > on_state_changed;
    
private :
    //* =====================================================================
    /// \brief Handle a negotiation that has been received in the active
    /// state.
    //* =====================================================================
    virtual std::vector<telnetpp::token> handle_subnegotiation(
        u8stream const &content);

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

}

#endif
