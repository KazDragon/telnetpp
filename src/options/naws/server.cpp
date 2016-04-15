#include "telnetpp/options/naws/server.hpp"
#include "telnetpp/options/naws/detail/protocol.hpp"

namespace telnetpp { namespace options { namespace naws {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
server::server()
  : server_option(telnetpp::options::naws::detail::option)
{
    on_state_changed.connect(
        [this](server_option::state)
        {
            return report_window_size();
        });
}

// ==========================================================================
// SET_WINDOW_SIZE
// ==========================================================================
std::vector<telnetpp::token> server::set_window_size(u16 width, u16 height)
{
    window_size_ = std::make_pair(width, height);
    return report_window_size();
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
std::vector<telnetpp::token> server::handle_subnegotiation(
    u8stream const &content)
{
    return {};
}

// ==========================================================================
// REPORT_WINDOW_SIZE
// ==========================================================================
std::vector<telnetpp::token> server::report_window_size()
{
    if (is_active() && window_size_.is_initialized())
    {
        return
        {
            telnetpp::element(telnetpp::subnegotiation(
                option(),
                {
                    telnetpp::u8(window_size_->first >> 8),
                    telnetpp::u8(window_size_->first & 0xFF),
                    telnetpp::u8(window_size_->second >> 8),
                    telnetpp::u8(window_size_->second & 0xFF)
                }))
        };
    }
    else
    {
        return {};
    }
}

}}}
