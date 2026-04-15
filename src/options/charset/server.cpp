#include "telnetpp/options/charset/server.hpp"

#include <utility>

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
// SELECT_CHARSET
// ==========================================================================
void server::select_charset(telnetpp::bytes charset)
{
    telnetpp::byte_storage content;
    content.reserve(1 + charset.size());

    content.push_back(detail::accepted);
    content.insert(content.end(), charset.begin(), charset.end());

    negotiated_charset_ = telnetpp::byte_storage{charset.begin(), charset.end()};
    write_subnegotiation(content);
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

    std::vector<telnetpp::byte_storage> advertised_charsets;
    telnetpp::byte_storage charset;

    for (auto const byte : data.subspan(2))
    {
        if (byte == separator)
        {
            if (!charset.empty())
            {
                advertised_charsets.push_back(charset);
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
        advertised_charsets.push_back(charset);
    }

    if (advertised_charsets.empty())
    {
        return;
    }

    advertised_charsets_ = std::move(advertised_charsets);
    negotiated_charset_.reset();
    on_charsets_advertised(advertised_charsets_);
}

}  // namespace telnetpp::options::charset
