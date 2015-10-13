#include "telnetpp/server_option.hpp"
#include "telnetpp/negotiation_router.hpp"
#include "telnetpp/protocol.hpp"
#include "telnetpp/subnegotiation_router.hpp"

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
// SET_ACTIVATABLE
// ==========================================================================
void server_option::set_activatable()
{
    activatable_ = true;
}

// ==========================================================================
// ACTIVATE
// ==========================================================================
std::vector<telnetpp::token> server_option::activate()
{
    if (state_ == state::inactive)
    {
        state_ = state::activating;
        return { telnetpp::element(
            telnetpp::negotiation(telnetpp::will, option_) 
        )};
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
        return { telnetpp::element(
            telnetpp::negotiation(telnetpp::wont, option_) 
        )};
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
            if (request == telnetpp::do_ && activatable_)
            {
                state_ = state::active;
                auto response = on_state_changed();
                response.insert(
                    response.begin(),
                    { telnetpp::element(
                        telnetpp::negotiation(telnetpp::will, option_)
                    )});
                
                return response;
            }
            else
            {
                return { telnetpp::element(
                    telnetpp::negotiation(telnetpp::wont, option_) 
                )};
            }
            
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
                return { telnetpp::element(
                    telnetpp::negotiation(telnetpp::will, option_) 
                )};
            }
            else
            {
                state_ = state::inactive;
                return { telnetpp::element(
                    telnetpp::negotiation(telnetpp::wont, option_) 
                )};
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

// ==========================================================================
// REGISTER_CLIENT_OPTION
// ==========================================================================
void register_server_option(
    server_option &option, 
    negotiation_router &neg_router, 
    subnegotiation_router &sub_router)
{
    register_route_from_negotiation_to_option(
        neg_router, telnetpp::do_, option);
    register_route_from_negotiation_to_option(
        neg_router, telnetpp::dont, option);
    register_route_from_subnegotiation_to_option(
        sub_router, option);
}

}
