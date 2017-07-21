#include "telnetpp/options/msdp/server.hpp"
#include "telnetpp/options/msdp/detail/decoder.hpp"
#include "telnetpp/options/msdp/detail/encoder.hpp"
#include "telnetpp/options/msdp/detail/protocol.hpp"

namespace telnetpp { namespace options { namespace msdp {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
server::server()
  : telnetpp::server_option(telnetpp::options::msdp::detail::option)
{
}

// ==========================================================================
// SEND
// ==========================================================================
std::vector<telnetpp::token> server::send(
    std::vector<telnetpp::options::msdp::variable> const &variables)
{
    auto result = detail::encode(variables);

    if (result.empty())
    {
        return {};
    }

    return { telnetpp::element{ telnetpp::subnegotiation(option(), result) } };
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
std::vector<telnetpp::token> server::handle_subnegotiation(
    byte_stream const &content)
{
    auto result = detail::decode(content);

    if (result.empty())
    {
        return {};
    }

    return on_receive(result);
}

}}}
