#include "telnetpp/options/new_environ/server.hpp"
#include "telnetpp/options/new_environ/detail/response_parser.hpp"
#include "telnetpp/options/new_environ/detail/stream.hpp"
#include "telnetpp/options/new_environ.hpp"

namespace telnetpp { namespace options { namespace new_environ {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
server::server()
  : server_option(telnetpp::options::new_environ::option)
{
    
}

// ==========================================================================
// REQUEST
// ==========================================================================
std::vector<token> server::request_variables(
    std::vector<request> const &requests)
{
    telnetpp::u8stream content;
    content.push_back(telnetpp::options::new_environ::send);
    
    for(auto &req : requests)
    {
        content.push_back(req.type);
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
std::vector<token> server::handle_subnegotiation(u8stream const &stream)
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
