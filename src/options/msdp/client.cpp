#include "telnetpp/options/msdp/client.hpp"
#include "telnetpp/options/msdp/detail/decoder.hpp"
#include "telnetpp/options/msdp/detail/encoder.hpp"

namespace telnetpp { namespace options { namespace msdp {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
client::client()
  : telnetpp::client_option(telnetpp::options::msdp::option)
{
}

// ==========================================================================
// SEND
// ==========================================================================
std::vector<telnetpp::token> client::send(
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
std::vector<telnetpp::token> client::handle_subnegotiation(
    u8stream const &content)
{
    auto result = detail::decode(content);

    if (result.empty())
    {
        return {};
    }

    return on_receive(result);
}

}}}
