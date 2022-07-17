#pragma once

#include "telnetpp/options/basic_server.hpp"
#include "telnetpp/options/naws/detail/protocol.hpp"
#include <optional>
#include <utility>

namespace telnetpp { namespace options { namespace naws {

//* =========================================================================
/// \brief An implementation of the server side of the Telnet NAWS option.
//* =========================================================================
class TELNETPP_EXPORT server 
  : public telnetpp::options::basic_server<detail::option>
{
public:
    using window_dimension = std::uint16_t;

    //* =====================================================================
    /// Constructor
    //* =====================================================================
    server();

    //* =====================================================================
    /// \brief Sets the window size that this option will report.
    //* =====================================================================
    template <typename Continuation>
    void set_window_size(
        window_dimension width,
        window_dimension height,
        Continuation &&cont)
    {
        window_size_ = std::make_pair(width, height);
        
        if (active())
        {
            report_window_size(cont);
        }
    }

private:
    //* =====================================================================
    /// \brief Report the window size.
    //* =====================================================================
    template <typename Continuation>
    void report_window_size(Continuation &&send)
    {
        if (window_size_.has_value())
        {
            telnetpp::byte const content[] = {
                byte(window_size_->first >> 8),
                byte(window_size_->first & 0xFF),
                byte(window_size_->second >> 8),
                byte(window_size_->second & 0xFF)
            };

            send(telnetpp::subnegotiation(option_code(), content));
        }
    }

    std::optional<std::pair<
        window_dimension, window_dimension
    >> window_size_;
};

}}}
