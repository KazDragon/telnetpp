#include "telnetpp/options/new_environ/client.hpp"
#include "telnetpp/options/new_environ/detail/protocol.hpp"
#include "telnetpp/options/new_environ/detail/response_parser.hpp"
#include "telnetpp/options/new_environ/detail/stream.hpp"

namespace telnetpp { namespace options { namespace new_environ {

namespace {

// ==========================================================================
// TYPE_TO_BYTE
// ==========================================================================
static byte type_to_byte(
    telnetpp::options::new_environ::variable_type const &type)
{
    return type == variable_type::var
         ? telnetpp::options::new_environ::detail::var
         : telnetpp::options::new_environ::detail::uservar;
}

}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
client::client()
  : client_option(telnetpp::options::new_environ::detail::option)
{
}

// ==========================================================================
// REQUEST
// ==========================================================================
std::vector<token> client::request_variables(
    std::vector<request> const &requests)
{
    telnetpp::byte_stream content;
    content.push_back(telnetpp::options::new_environ::detail::send);

    for(auto &req : requests)
    {
        content.push_back(type_to_byte(req.type));
        detail::append_escaped(content, req.name);
    }

    return {
        telnetpp::element {
            telnetpp::subnegotiation(option(), content)
        }
    };
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
std::vector<token> client::handle_subnegotiation(byte_stream const &stream)
{
    std::vector<token> responses;

    for (auto const &change : detail::parse_responses(stream))
    {
        auto const &response = on_variable_changed(change);
        responses.insert(responses.end(), response.begin(), response.end());
    }


    return responses;
}

}}}
