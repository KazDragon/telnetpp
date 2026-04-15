#include "telnetpp/options/charset/server.hpp"

namespace telnetpp::options::charset {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
server::server(telnetpp::session &sess) noexcept
  : telnetpp::server_option(sess, detail::option)
{
}

// ==========================================================================
// REQUEST_CHARSETS
// ==========================================================================
void server::request_charsets()
{
    static constexpr telnetpp::byte const request_content[] = {
        detail::request,
        ';'};

    write_subnegotiation(request_content);
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
void server::handle_subnegotiation(telnetpp::bytes /*data*/)
{
}

}  // namespace telnetpp::options::charset
