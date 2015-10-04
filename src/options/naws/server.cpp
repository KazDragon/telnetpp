#include "telnetpp/options/naws/server.hpp"
#include "telnetpp/options/naws.hpp"
#include "telnetpp/protocol.hpp"

namespace telnetpp { namespace options { namespace naws {

// ==========================================================================
// ACTIVATE
// ==========================================================================
std::vector<telnetpp::token> server::activate()
{
    return { telnetpp::negotiation(option, telnetpp::will) };
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
    return false;
}

// ==========================================================================
// NEGOTIATE
// ==========================================================================
std::vector<telnetpp::token> server::negotiate(telnetpp::u8 request)
{
    return { telnetpp::negotiation(option, telnetpp::wont) };
}


}}}
