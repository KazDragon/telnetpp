#pragma once

#include "telnetpp/core.hpp"
#include "telnetpp/element.hpp"
#include <boost/signals2.hpp>
#include <functional>

namespace telnetpp { namespace detail {

//* =========================================================================
/// \exclude
//* =========================================================================
template <
    telnetpp::command_type LocalPositive,
    telnetpp::command_type LocalNegative,
    telnetpp::command_type RemotePositive,
    telnetpp::command_type RemoteNegative>
class option
{
public:
    using continuation = std::function<
        void (telnetpp::element const &)
    >;

    //* =====================================================================
    /// Constructor
    //* =====================================================================
    explicit option(telnetpp::option_type code)
      : code_(code)
    {
    }

    //* =====================================================================
    /// Destructor
    //* =====================================================================
    virtual ~option() = default;

    //* =====================================================================
    /// Returns the code for the option.
    //* =====================================================================
    telnetpp::option_type code() const
    {
        return code_;
    }

    //* =====================================================================
    /// \brief Returns whether the option is active.
    //* =====================================================================
    bool active() const
    {
        return state_ == internal_state::active;
    }

    //* =====================================================================
    /// \brief Begins the activation process for the option.
    /// \param send a continuation that sends the Telnet elements that are
    /// emitted by this process.
    //* =====================================================================
    template <class Continuation>
    void activate(Continuation &&send)
    {
        switch (state_)
        {
            case internal_state::inactive:
                send(telnetpp::negotiation{LocalPositive, code_});
                state_ = internal_state::activating;
                break;

            case internal_state::activating:
                break;

            case internal_state::active:
                on_state_changed(send);
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
    template <class Continuation>
    void deactivate(Continuation &&send)
    {
        switch (state_)
        {
            case internal_state::active:
                send(telnetpp::negotiation{LocalNegative, code_});
                state_ = internal_state::deactivating;
                on_state_changed(send);
                break;

            case internal_state::deactivating:
                break;

            case internal_state::inactive:
                on_state_changed(send);
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
    template <class Continuation>
    void negotiate(telnetpp::negotiation_type neg, Continuation &&send)
    {
        switch (state_)
        {
            case internal_state::inactive:
                if (neg == RemotePositive)
                {
                    send(telnetpp::negotiation{LocalPositive, code_});
                    state_ = internal_state::active;
                    on_state_changed(send);
                }
                else
                {
                    send(telnetpp::negotiation{LocalNegative, code_});
                }
                break;

            case internal_state::activating:
                if (neg == RemotePositive)
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
                if (neg == RemotePositive)
                {
                    send(telnetpp::negotiation{LocalPositive, code_});
                }
                else
                {
                    send(telnetpp::negotiation{LocalNegative, code_});
                    state_ = internal_state::inactive;
                    on_state_changed(send);
                }
                break;

            case internal_state::deactivating:
                if (neg == RemotePositive)
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
    /// \brief Negotiate with the option.
    /// This should be called when a subnegotiation sequence has been received
    /// from the remote.
    //* =====================================================================
    template <typename Continuation>
    void subnegotiate(
        telnetpp::bytes content,
        Continuation &&cont)
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

}}
