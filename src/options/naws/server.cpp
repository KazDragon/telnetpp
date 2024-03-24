#include "telnetpp/options/naws/server.hpp"

namespace telnetpp::options::naws {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
server::server(telnetpp::session &sess) : basic_server(sess)
{
    on_state_changed.connect([this]() {
        if (active())
        {
            report_window_size();
        }
    });
}

// ==========================================================================
// SET_WINDOW_SIZE
// ==========================================================================
void server::set_window_size(window_dimension width, window_dimension height)
{
    window_size_ = std::make_pair(width, height);

    if (active())
    {
        report_window_size();
    }
}

// ==========================================================================
// REPORT_WINDOW_SIZE
// ==========================================================================
void server::report_window_size()
{
    if (window_size_.has_value())
    {
        telnetpp::byte const content[] = {
            static_cast<byte>(window_size_->first >> 8),
            static_cast<byte>(window_size_->first & 0xFF),
            static_cast<byte>(window_size_->second >> 8),
            static_cast<byte>(window_size_->second & 0xFF)};

        write_subnegotiation(content);
    }
}

}  // namespace telnetpp::options::naws
