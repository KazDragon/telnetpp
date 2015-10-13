#include "telnetpp/client_option.hpp"
#include "telnetpp/negotiation_router.hpp"
#include "telnetpp/protocol.hpp"
#include "telnetpp/subnegotiation_router.hpp"

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
std::vector<telnetpp::token_pass> client_option::activate()
{
    if (state_ == state::inactive)
    {
        state_ = state::activating;
        return { telnetpp::token {
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
std::vector<telnetpp::token_pass> client_option::deactivate()
{
    if (state_ == state::inactive)
    {
        return on_state_changed();
    }
    
    if (state_ == state::active)
    {
        state_ = state::deactivating;
        return { telnetpp::token{ 
            telnetpp::negotiation(telnetpp::dont, option_) } 
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
std::vector<telnetpp::token_pass> client_option::negotiate(telnetpp::u8 request)
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
                    { telnetpp::token {
                        telnetpp::negotiation(telnetpp::do_, option_) } 
                    });
                
                return response;
            }
            else
            {
                return { telnetpp::token {
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
                return { telnetpp::token {
                    telnetpp::negotiation(telnetpp::do_, option_) }
                };
            }
            else
            {
                state_ = state::inactive;
                return { telnetpp::token { 
                    telnetpp::negotiation(telnetpp::dont, option_) }
                };
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
std::vector<telnetpp::token_pass> client_option::subnegotiate(
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
std::vector<telnetpp::token_pass> client_option::handle_subnegotiation(
    std::vector<telnetpp::u8> const&)
{
    // By default, do nothing.   
    return {};
}
        
// ==========================================================================
// REGISTER_CLIENT_OPTION
// ==========================================================================
void register_client_option(
    client_option &option, 
    negotiation_router &neg_router, 
    subnegotiation_router &sub_router)
{
    register_route_from_negotiation_to_option(
        neg_router, telnetpp::will, option);
    register_route_from_negotiation_to_option(
        neg_router, telnetpp::wont, option);
    register_route_from_subnegotiation_to_option(
        sub_router, option);
}

}
