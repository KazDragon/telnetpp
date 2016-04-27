#include "telnetpp/options/mccp/server.hpp"
#include "telnetpp/options/mccp/mccp.hpp"
#include "telnetpp/options/mccp/detail/protocol.hpp"

namespace telnetpp { namespace options { namespace mccp {

namespace {
    
static auto const begin_compression_sequence = std::vector<telnetpp::token> {
    telnetpp::element(
        telnetpp::subnegotiation(detail::option, {})),
    boost::any(telnetpp::options::mccp::begin_compression{})
};

static auto const end_compression_sequence = std::vector<telnetpp::token> {
    boost::any(telnetpp::options::mccp::end_compression{})
};

}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
server::server()
  : server_option(detail::option),
    compression_requested_(false)
{
    on_state_changed.connect(
        [this](auto const state) -> std::vector<telnetpp::token>
        {
            if (state == telnetpp::server_option::state::active
             && compression_requested_)
            {
                return begin_compression_sequence;
            }
            else
            {
                return {};
            }
        });
}

// ==========================================================================
// BEGIN_COMPRESSION
// ==========================================================================
std::vector<telnetpp::token> server::begin_compression()
{
    if (is_active())
    {
        return begin_compression_sequence;
    }
    else
    {
        compression_requested_ = true;
        return {};
    }
}

// ==========================================================================
// END_COMPRESSION
// ==========================================================================
std::vector<telnetpp::token> server::end_compression()
{
    if (is_active())
    {
        return end_compression_sequence;
    }
    else
    {
        compression_requested_ = false;
        return {};
    }
}

}}}
