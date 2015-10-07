#include "telnetpp/server_option.hpp"
#include "telnetpp/protocol.hpp"

namespace telnetpp {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
server_option::server_option(u8 option)
  : option_(option)
{
}

// ==========================================================================
// OPTION
// ==========================================================================
u8 server_option::option() const
{
    return option_;
}

// ==========================================================================
// ACTIVATE
// ==========================================================================
std::vector<telnetpp::token> server_option::activate()
{
    if (state_ == state::inactive)
    {
        state_ = state::activating;
        return { telnetpp::negotiation(telnetpp::will, option_) };
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
std::vector<telnetpp::token> server_option::deactivate()
{
    if (state_ == state::inactive)
    {
        return on_state_changed();
    }
    
    if (state_ == state::active)
    {
        state_ = state::deactivating;
        return { telnetpp::negotiation(telnetpp::wont, option_) };
    }
    else
    {
        return {};
    }
}

// ==========================================================================
// IS_ACTIVE
// ==========================================================================
bool server_option::is_active() const
{
    return state_ == state::active;
}

// ==========================================================================
// NEGOTIATE
// ==========================================================================
std::vector<telnetpp::token> server_option::negotiate(telnetpp::u8 request)
{
    switch(state_)
    {
        case state::inactive :
            return { 
                telnetpp::negotiation(telnetpp::wont, option_)
            };
            
        case state::activating :
            if (request == telnetpp::do_)
            {
                state_ = state::active;
                return on_state_changed();
            }
            else
            {
                state_ = state::inactive;
                return on_state_changed();
            }

        case state::active :
            if (request == telnetpp::do_)
            {
                return { telnetpp::negotiation(telnetpp::will, option_) };
            }
            else
            {
                state_ = state::inactive;
                return { telnetpp::negotiation(telnetpp::wont, option_) };
            }
            
        case state::deactivating :
            if (request == telnetpp::do_)
            {
                // NOTE: This is technically unspecified.  No client would send
                // a DO when we're already active, and they're not allowed to
                // send a DO after receiving a WONT.  But to be nice, we'll
                // re-activate.
                state_ = state::active;
                return on_state_changed();
            }
            else if (request == telnetpp::dont)
            {
                state_ = state::inactive;
                return on_state_changed();
            }
    }
    
    return {};
}

// ==========================================================================
// SUBNEGOTIATE
// ==========================================================================
std::vector<telnetpp::token> server_option::subnegotiate(
    std::vector<telnetpp::u8> const &content)
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
std::vector<telnetpp::token> server_option::handle_subnegotiation(
    std::vector<telnetpp::u8> const &)
{
    // By default, do nothing.
    return {};
}

}
