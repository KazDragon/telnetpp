#include "telnetpp/options/new_environ/client.hpp"

#include "telnetpp/options/new_environ/detail/for_each_response.hpp"
#include "telnetpp/options/new_environ/detail/protocol.hpp"
#include "telnetpp/options/new_environ/detail/response_parser_helper.hpp"
#include "telnetpp/options/new_environ/detail/stream.hpp"

#include <numeric>

namespace telnetpp::options::new_environ {

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
    byte_storage request_content{detail::send};

    for (auto const &request : reqs)
    {
        request_content.push_back(detail::type_to_byte(request.type));
        detail::append_escaped(request_content, request.name);
    }

    write_subnegotiation(request_content);
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
void client::handle_subnegotiation(telnetpp::bytes data)
{
    detail::for_each_response(
        data, [&](response const &rsp) { on_variable_changed(rsp); });
}

}  // namespace telnetpp::options::new_environ
