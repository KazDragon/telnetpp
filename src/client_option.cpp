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
// ACTIVATE
// ==========================================================================
std::vector<telnetpp::token> client_option::activate()
{
    if (state_ == state::inactive)
    {
        state_ = state::activating;
        return { telnetpp::negotiation(telnetpp::do_, option_) };
    }
    else
    {
        if (state_ == state::active)
        {
            on_state_changed();
        }
        
        return {};
    }
}

// ==========================================================================
// DEACTIVATE
// ==========================================================================
std::vector<telnetpp::token> client_option::deactivate()
{
    if (state_ == state::inactive)
    {
        on_state_changed();
        return {};
    }
    
    if (state_ == state::active)
    {
        state_ = state::deactivating;
        return { telnetpp::negotiation(telnetpp::dont, option_) };
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
            return { 
                telnetpp::negotiation(telnetpp::dont, option_)
            };
            
        case state::activating :
            if (request == telnetpp::will)
            {
                state_ = state::active;
                on_state_changed();
            }
            else
            {
                state_ = state::inactive;
                on_state_changed();
            }
            
            return {};

        case state::active :
            if (request == telnetpp::will)
            {
                return { telnetpp::negotiation(telnetpp::do_, option_) };
            }
            else
            {
                state_ = state::inactive;
                return { telnetpp::negotiation(telnetpp::dont, option_) };
            }
            
        case state::deactivating :
            if (request == telnetpp::will)
            {
                // NOTE: This is technically unspecified.  No client would send
                // a WILL when we're already active, and they're not allowed to
                // send a WILL after receiving a DONT.  But to be nice, we'll
                // re-activate.
                state_ = state::active;
                on_state_changed();
            }
            else if (request == telnetpp::wont)
            {
                state_ = state::inactive;
                on_state_changed();
            }
                
            return {};
    }
}

// ==========================================================================
// SUBNEGOTIATE
// ==========================================================================
void client_option::subnegotiate(std::vector<telnetpp::u8> const &content)
{
    if (state_ == state::active)
    {
        handle_subnegotiation(content);
    }
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
void client_option::handle_subnegotiation(std::vector<telnetpp::u8> const&)
{
    // By default, do nothing.    
}
        

}
