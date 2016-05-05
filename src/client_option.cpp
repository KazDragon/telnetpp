#include "telnetpp/client_option.hpp"
#include "telnetpp/protocol.hpp"

namespace telnetpp {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
client_option::client_option(u8 option)
  : option_(option)
{
}

// ==========================================================================
// OPTION
// ==========================================================================
u8 client_option::option() const
{
    return option_;
}

// ==========================================================================
// SET_ACTIVATABLE
// ==========================================================================
void client_option::set_activatable()
{
    activatable_ = true;
}

// ==========================================================================
// ACTIVATE
// ==========================================================================
std::vector<telnetpp::token> client_option::activate()
{
    if (state_ == state::inactive)
    {
        state_ = state::activating;
        return { telnetpp::element {
            telnetpp::negotiation(telnetpp::do_, option_) }
        };
    }
    else
    {
        if (state_ == state::active)
        {
            return on_state_changed();
        }
        else
        {
            return {};
        }
    }
}

// ==========================================================================
// DEACTIVATE
// ==========================================================================
std::vector<telnetpp::token> client_option::deactivate()
{
    if (state_ == state::inactive)
    {
        return on_state_changed();
    }

    if (state_ == state::active)
    {
        state_ = state::deactivating;

        return {
            telnetpp::element(telnetpp::negotiation(telnetpp::dont, option_))
        };
    }
    else
    {
        return {};
    }
}

// ==========================================================================
// IS_ACTIVE
// ==========================================================================
bool client_option::is_active() const
{
    return state_ == state::active;
}

// ==========================================================================
// NEGOTIATE
// ==========================================================================
std::vector<telnetpp::token> client_option::negotiate(telnetpp::u8 request)
{
    switch(state_)
    {
        case state::inactive :
            if (request == telnetpp::will && activatable_)
            {
                state_ = state::active;
                auto response = on_state_changed();
                response.insert(
                    response.begin(),
                    { telnetpp::element {
                        telnetpp::negotiation(telnetpp::do_, option_) }
                    });

                return response;
            }
            else
            {
                return { telnetpp::element {
                    telnetpp::negotiation(telnetpp::dont, option_) }
                };
            }

        case state::activating :
            if (request == telnetpp::will)
            {
                state_ = state::active;
                return on_state_changed();
            }
            else if (request == telnetpp::wont)
            {
                state_ = state::inactive;
                return on_state_changed();
            }

            return {};

        case state::active :
            if (request == telnetpp::will)
            {
                return { telnetpp::element {
                    telnetpp::negotiation(telnetpp::do_, option_) }
                };
            }
            else if (request == telnetpp::wont)
            {
                state_ = state::inactive;

                auto response = on_state_changed();
                response.insert(
                    response.begin(),
                    telnetpp::element {
                        telnetpp::negotiation(telnetpp::dont, option_)
                    });

                return response;
            }

        case state::deactivating :
            if (request == telnetpp::will)
            {
                // NOTE: This is technically unspecified.  No client would send
                // a WILL when we're already active, and they're not allowed to
                // send a WILL after receiving a DONT.  But to be nice, we'll
                // re-activate.
                state_ = state::active;
                return on_state_changed();
            }
            else if (request == telnetpp::wont)
            {
                state_ = state::inactive;
                return on_state_changed();
            }

            return {};
    }
}

// ==========================================================================
// SUBNEGOTIATE
// ==========================================================================
std::vector<telnetpp::token> client_option::subnegotiate(
    u8stream const &content)
{
    if (state_ == state::active)
    {
        return handle_subnegotiation(content);
    }
    else
    {
        return {};
    }
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
std::vector<telnetpp::token> client_option::handle_subnegotiation(
    u8stream const&)
{
    // By default, do nothing.
    return {};
}

}
