#pragma once

#include "telnetpp/session.hpp"
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
class option
{
 public:
  static constexpr auto local_positive = LocalPositive;
  static constexpr auto local_negative = LocalNegative;
  static constexpr auto remote_positive = RemotePositive;
  static constexpr auto remote_negative = RemoteNegative;

  //* =====================================================================
  /// Destructor
  //* =====================================================================
  virtual ~option() = default;

  //* =====================================================================
  /// Returns the code for the option.
  //* =====================================================================
  [[nodiscard]] constexpr telnetpp::option_type option_code() const noexcept
  {
    return code_;
  }

  //* =====================================================================
  /// \brief Returns whether the option is active.
  //* =====================================================================
  [[nodiscard]] constexpr bool active() const noexcept
  {
    return state_ == internal_state::active;
  }

  //* =====================================================================
  /// \brief Begins the activation process for the option.
  //* =====================================================================
  constexpr void activate()
  {
    switch (state_)
    {
      case internal_state::inactive:
        state_ = internal_state::activating;
        write_negotiation(local_positive);
        break;

      case internal_state::activating:
        break;

      case internal_state::active:
        on_state_changed();
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
  //* =====================================================================
  constexpr void deactivate()
  {
    switch (state_)
    {
      case internal_state::active:
        state_ = internal_state::deactivating;
        write_negotiation(local_negative);
        break;

      case internal_state::activating:
        break;

      case internal_state::inactive:
        on_state_changed();
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
  constexpr void negotiate(telnetpp::negotiation_type neg)
  {
    switch (state_)
    {
      case internal_state::inactive:
        if (neg == remote_positive)
        {
          state_ = internal_state::active;
          write_negotiation(local_positive);
          on_state_changed();
        }
        else
        {
          write_negotiation(local_negative);
        }
        break;

      case internal_state::activating:
        if (neg == remote_positive)
        {
          state_ = internal_state::active;
          on_state_changed();
        }
        else
        {
          state_ = internal_state::inactive;
          on_state_changed();
        }
        break;

      case internal_state::active:
        if (neg == remote_positive)
        {
          write_negotiation(local_positive);
        }
        else
        {
          state_ = internal_state::inactive;
          on_state_changed();
          write_negotiation(local_negative);
        }
        break;

      case internal_state::deactivating:
        if (neg == remote_positive)
        {
          state_ = internal_state::active;
          on_state_changed();
        }
        else
        {
          state_ = internal_state::inactive;
          on_state_changed();
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
  constexpr void subnegotiate(telnetpp::bytes content)
  {
    if (state_ == internal_state::active)
    {
      handle_subnegotiation(content);
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
  boost::signals2::signal<void()> on_state_changed;  // NOLINT

 protected:
  //* =====================================================================
  /// Constructor
  //* =====================================================================
  constexpr explicit option(
      telnetpp::session &sess, telnetpp::option_type code) noexcept
    : session_{sess}, code_{code}
  {
  }

  //* =====================================================================
  /// \brief Write plain text to the session
  //* =====================================================================
  void write_text(telnetpp::bytes content)
  {
    session_.write(content);
  }

  //* =====================================================================
  /// \brief Write a subnegotiation to the session
  //* =====================================================================
  void write_subnegotiation(telnetpp::bytes content)
  {
    session_.write(telnetpp::subnegotiation{code_, content});
  }

 private:
  //* =====================================================================
  /// \brief Write a negotiation to the session
  //* =====================================================================
  void write_negotiation(telnetpp::negotiation_type neg)
  {
    session_.write(telnetpp::negotiation{neg, code_});
  }

  //* =====================================================================
  /// \brief Called when a subnegotiation is received while the option is
  /// active.  Override for option-specific functionality.
  //* =====================================================================
  virtual void handle_subnegotiation(telnetpp::bytes data) = 0;

  enum class internal_state
  {
    inactive,
    activating,
    active,
    deactivating,
  };

  telnetpp::session &session_;
  telnetpp::option_type code_;
  internal_state state_ = internal_state::inactive;
};

}  // namespace telnetpp
