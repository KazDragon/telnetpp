#pragma once

#include "telnetpp/element.hpp"
#include <boost/signals2.hpp>
#include <vector>

namespace telnetpp {

//* =========================================================================
/// \brief A class that represents a Telnet option's server side.  That is,
/// the side that received DO and DONT negotiations and sends WILL and WONT
/// negotiations.
///
/// \par Server vs. Client
/// Note that the usage of server in this context may disagree with a
/// particular option's RFC specification.  The determination of what is a
/// client and what is a server is not rigorously applies throughout the
/// RFCs, so consider this merely an implementation detail of this library.
/// \par Usage
/// server_option provides a general interface to enable the implementation
/// of the server sides of all Telnet options. There are two customization
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
class TELNETPP_EXPORT server_option {
public :
    //* =====================================================================
    /// \brief An enumeration of the different states in which this option
    /// can be.
    //* =====================================================================
    enum class state
    {
        inactive,
        activating,
        active,
        deactivating,
    };

    //* =====================================================================
    /// \brief Returns the option code.
    //* =====================================================================
    option_type option() const;

    //* =====================================================================
    /// \brief Flags the option as remotely activatable.
    ///
    /// Allows the option to be activated by the remote end.  That is,
    /// if the option receives a DO negotiation, it will respond with a
    /// WILL negotiation.  An option that has not have this function called
    /// will instead respond with a WONT negotiation.
    //* =====================================================================
    void set_activatable();

    //* =====================================================================
    /// \brief Activates the option; sends a WILL negotiation.
    //* =====================================================================
    std::vector<telnetpp::token> activate();

    //* =====================================================================
    /// \brief Deactivates the option; sends a WONT negotiation.
    //* =====================================================================
    std::vector<telnetpp::token> deactivate();

    //* =====================================================================
    /// \brief Returns true iff the option is active.
    //* =====================================================================
    bool is_active() const;

    //* =====================================================================
    /// \brief Makes a request of the option and returns the response from
    /// that request.
    //* =====================================================================
    std::vector<telnetpp::token> negotiate(negotiation_type request);

    //* =====================================================================
    /// \brief Send a subnegotiation to the option and returns the response
    /// from that subnegotiation.
    //* =====================================================================
    std::vector<telnetpp::token> subnegotiate(
        telnetpp::byte_stream const &content);

    //* =====================================================================
    /// \brief A signal that you can connect to in order to detect changes
    /// in the state of the option.
    /// \par Usage
    /// \code
    /// my_option.on_state_changed.connect(
    ///     [](telnetpp::server_option::state new_state)
    ///         -> std::vector<telnetpp::token>
    ///     {
    ///         // Do something with the state change
    ///         std::cout << "state is now: " << new_state << "\n";
    ///         // return some vector of tokens, or an empty vector
    ///         return {};
    ///     });
    /// \endcode
    /// \see telnetpp::server_option::state
    //* =====================================================================
    boost::signals2::signal<
        std::vector<telnetpp::token> (state new_state),
        token_combiner
    > on_state_changed;

protected :
    //* =====================================================================
    /// \brief Constructor
    /// \param option The Option Code of this option.
    //* =====================================================================
    explicit server_option(option_type option);

private :
    //* =====================================================================
    /// \brief Handle a negotiation that has been received in the active
    /// state.
    //* =====================================================================
    virtual std::vector<telnetpp::token> handle_subnegotiation(
        telnetpp::byte_stream const &content) = 0;

    state       state_       = state::inactive;
    bool        activatable_ = false;
    option_type option_;
};

}
