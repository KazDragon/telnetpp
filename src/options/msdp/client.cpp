#include "telnetpp/options/msdp/client.hpp"
// #include "telnetpp/options/msdp/detail/decoder.hpp"
// #include "telnetpp/options/msdp/detail/encoder.hpp"
#include "telnetpp/options/msdp/detail/protocol.hpp"

namespace telnetpp { namespace options { namespace msdp {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
client::client()
  : telnetpp::client_option(telnetpp::options::msdp::detail::option)
{
}

/*
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
*/

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
void client::handle_subnegotiation(
    telnetpp::bytes data,
    continuation const &cont)
{
    /*
    auto result = detail::decode(content);

    if (result.empty())
    {
        return {};
    }

    return on_receive(result);
    */
}

}}}
