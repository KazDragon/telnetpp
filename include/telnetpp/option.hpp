#pragma once

#include "telnetpp/core.hpp"
#include "telnetpp/element.hpp"
#include <boost/signals2.hpp>
#include <functional>

namespace telnetpp {

//* =========================================================================
/// \brief An class that encapsulates one side of a Telnet option.
///
/// A Telnet option comprises two essential parts: an activation sequence,
/// whereby a client and a server agree upon whether the option is active
/// or not, and a subnegotiation handler in which the option-specific
/// protocol is exchanged.
///
/// \par Inheritance
///
/// For most options, only handle_subnegotiation need be overridden.  This
/// is where the option-specific protocol contained in the subnegotiations
/// is received.
///
/// An option may require auxiliary classes in order to be useful.  See
/// the documentation accompanying those options for more information.
///
/// \par Usage
///
/// The easiest way to use an option is to install it in a session.  A
/// session is used to automatically route relevant messages to and from
/// an option.  See telnetpp::session for more information.
///
/// \par Note
///
/// Most functions that telnetpp::option exposes could potentially require
/// that data be transmitted to the remote.  To allow the user to specify
/// exactly how that occurs, telnetpp::option uses continuations: functions
/// where the caller specifies how the result of the operation should be
/// used.
//* =========================================================================
template <
    telnetpp::command_type LocalPositive,
    telnetpp::command_type LocalNegative,
    telnetpp::command_type RemotePositive,
    telnetpp::command_type RemoteNegative>
class TELNETPP_EXPORT option
{
public:
    using continuation = std::function<void (telnetpp::element const &)>;

    static constexpr auto local_positive  = LocalPositive;
    static constexpr auto local_negative  = LocalNegative;
    static constexpr auto remote_positive = RemotePositive;
    static constexpr auto remote_negative = RemoteNegative;

    //* =====================================================================
    /// Destructor
    //* =====================================================================
    virtual ~option() = default;

    //* =====================================================================
    /// Returns the code for the option.
    //* =====================================================================
    constexpr telnetpp::option_type option_code() const noexcept
    {
        return code_;
    }

    //* =====================================================================
    /// \brief Returns whether the option is active.
    //* =====================================================================
    constexpr bool active() const noexcept
    {
        return state_ == internal_state::active;
    }

    //* =====================================================================
    /// \brief Begins the activation process for the option.
    /// \param send a continuation that sends the Telnet elements that are
    /// emitted by this process.
    //* =====================================================================
    template <typename Continuation>
    constexpr void activate(Continuation &&send)
    {
        switch (state_)
        {
            case internal_state::inactive:
                state_ = internal_state::activating;
                send(telnetpp::negotiation{local_positive, code_});
                break;

            case internal_state::activating:
                break;

            case internal_state::active:
                on_state_changed(send);
                break;

            case internal_state::deactivating:
                break;

            default:
                assert(!"Unimplemented");
                break;
        }
    }

    //* =====================================================================
    /// \brief Begins the deactivation process for the option.
    /// \param send a continuation that sends the Telnet elements that are
    /// emitted by this process.
    //* =====================================================================
    template <typename Continuation>
    constexpr void deactivate(Continuation &&send)
    {
        switch (state_)
        {
            case internal_state::active:
                state_ = internal_state::deactivating;
                send(telnetpp::negotiation{local_negative, code_});
                break;

            case internal_state::activating:
                break;

            case internal_state::inactive:
                on_state_changed(send);
                break;

            case internal_state::deactivating:
                break;

            default:
                assert(!"Unimplemented");
                break;
        }
    }

    //* =====================================================================
    /// \brief Negotiate with the option.
    /// This should be called when the remote side either initiates a 
    /// negotiation request or responds to an ongoing request.
    //* =====================================================================
    template <typename Continuation>
    constexpr void negotiate(
        telnetpp::negotiation_type neg, 
        Continuation &&send)
    {
        switch (state_)
        {
            case internal_state::inactive:
                if (neg == remote_positive)
                {
                    state_ = internal_state::active;
                    send(telnetpp::negotiation{local_positive, code_});
                    on_state_changed(send);
                }
                else
                {
                    send(telnetpp::negotiation{local_negative, code_});
                }
                break;

            case internal_state::activating:
                if (neg == remote_positive)
                {
                    state_ = internal_state::active;
                    on_state_changed(send);
                }
                else
                {
                    state_ = internal_state::inactive;
                    on_state_changed(send);
                }
                break;

            case internal_state::active:
                if (neg == remote_positive)
                {
                    send(telnetpp::negotiation{local_positive, code_});
                }
                else
                {
                    state_ = internal_state::inactive;
                    on_state_changed(send);
                    send(telnetpp::negotiation{local_negative, code_});
                }
                break;

            case internal_state::deactivating:
                if (neg == remote_positive)
                {
                    state_ = internal_state::active;
                    on_state_changed(send);
                }
                else
                {
                    state_ = internal_state::inactive;
                    on_state_changed(send);
                }
                break;

            default:
                assert(!"Unimplemented");
                break;
        }
    }

    //* =====================================================================
    /// \brief Subnegotiate with the option.
    /// This should be called when a subnegotiation sequence has been received
    /// from the remote.
    //* =====================================================================
    constexpr void subnegotiate(
        telnetpp::bytes content, continuation const &cont)
    {
        if (state_ == internal_state::active)
        {
            handle_subnegotiation(content, cont);
        }
    }

    //* =====================================================================
    /// \fn on_state_changed.connect
    /// \brief A signal that is emitted whenever there is a change in the 
    /// state of the option.
    ///
    /// This will be emitted when a solicited negotiation is complete, 
    /// whatever the outcome, or whenever an unsolicited negotiation results
    /// in the option changing state.
    ///
    /// The parameter of the signal is a continuation that can be called to
    /// send any Telnet elements that are emitted by this process.
    //* =====================================================================
    boost::signals2::signal<void (continuation const &)> on_state_changed;

protected:
    //* =====================================================================
    /// Constructor
    //* =====================================================================
    constexpr explicit option(telnetpp::option_type code) noexcept
      : code_(code)
    {
    }

private:
    //* =====================================================================
    /// \brief Called when a subnegotiation is received while the option is
    /// active.  Override for option-specific functionality.
    //* =====================================================================
    virtual void handle_subnegotiation(
        telnetpp::bytes data,
        continuation const &cont) = 0;

    enum class internal_state
    {
        inactive,
        activating,
        active,
        deactivating,
    };

    telnetpp::option_type code_;
    internal_state        state_ = internal_state::inactive;
};

}
