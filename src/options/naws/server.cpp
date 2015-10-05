#include "telnetpp/options/naws/server.hpp"
#include "telnetpp/options/naws.hpp"
#include "telnetpp/protocol.hpp"

namespace telnetpp { namespace options { namespace naws {

// ==========================================================================
// ACTIVATE
// ==========================================================================
std::vector<telnetpp::token> server::activate()
{
    if (state_ == state::inactive)
    {
        state_ = state::activating;
        return { 
            telnetpp::negotiation(
                telnetpp::will, telnetpp::options::naws::option) 
        };
    }
    else
    {
        return {};
    }
}

// ==========================================================================
// DEACTIVATE
// ==========================================================================
std::vector<telnetpp::token> server::deactivate()
{
    return {};
}

// ==========================================================================
// IS_ACTIVE
// ==========================================================================
bool server::is_active() const
{
    return state_ == state::active;
}

// ==========================================================================
// NEGOTIATE
// ==========================================================================
std::vector<telnetpp::token> server::negotiate(telnetpp::u8 request)
{
    switch(state_)
    {
        case state::inactive :
            return { 
                telnetpp::negotiation(
                    telnetpp::wont, telnetpp::options::naws::option)
            };
            
        case state::activating :
            if (request == telnetpp::do_)
            {
                state_ = state::active;
            }
            else
            {
                state_ = state::inactive;
            }
            
            return {};

        case state::active :
            if (request == telnetpp::do_)
            {
                return { telnetpp::negotiation(telnetpp::will, option) };
            }
            
            return {};
            
        case state::deactivating :
            return {};
    }
}


}}}
