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
void server::handle_subnegotiation(telnetpp::bytes data)
{
    if (data.size() < 2 || data[0] != detail::request)
    {
        return;
    }

    auto const separator = data[1];

    advertised_charsets_.clear();
    negotiated_charset_.reset();

    telnetpp::byte_storage charset;

    for (auto const byte : data.subspan(2))
    {
        if (byte == separator)
        {
            if (!charset.empty())
            {
                advertised_charsets_.push_back(charset);
                charset.clear();
            }
        }
        else
        {
            charset.push_back(byte);
        }
    }

    if (!charset.empty())
    {
        advertised_charsets_.push_back(charset);
    }

    on_charsets_advertised(advertised_charsets_);
}

}  // namespace telnetpp::options::charset
