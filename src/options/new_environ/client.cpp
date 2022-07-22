#include "telnetpp/options/new_environ/client.hpp"
#include "telnetpp/options/new_environ/detail/for_each_response.hpp"
#include "telnetpp/options/new_environ/detail/protocol.hpp"
#include "telnetpp/options/new_environ/detail/stream.hpp"
#include <numeric>

namespace telnetpp { namespace options { namespace new_environ {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
client::client(telnetpp::session &sess) noexcept
  : client_option(sess, telnetpp::options::new_environ::detail::option)
{
}

// ==========================================================================
// REQUEST_VARIABLES
// ==========================================================================
void client::request_variables(requests const &reqs)
{
    auto request_content = std::accumulate(
        reqs.begin(),
        reqs.end(),
        byte_storage{detail::send},
        [](byte_storage &content, request const &req) -> byte_storage &
        {
            content.push_back(detail::type_to_byte(req.type));
            detail::append_escaped(content, req.name);
            return content;
        });

    write_subnegotiation(request_content);
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
void client::handle_subnegotiation(telnetpp::bytes data)
{
    detail::for_each_response(
        data,
        [&](response const &rsp)
        {
            on_variable_changed(rsp);
        });
}

}}}
