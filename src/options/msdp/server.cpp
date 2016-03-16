#include "telnetpp/options/msdp/server.hpp"
#include "telnetpp/options/msdp/detail/encoder.hpp"

namespace telnetpp { namespace options { namespace msdp {

server::server()
  : telnetpp::server_option(telnetpp::options::msdp::option)
{
}

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

}}}
