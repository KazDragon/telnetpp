#ifndef TELNETPP_ROUTING_VISITOR_HPP_
#define TELNETPP_ROUTING_VISITOR_HPP_

#include "telnetpp/command_router.hpp"
#include "telnetpp/negotiation_router.hpp"
#include "telnetpp/subnegotiation_router.hpp"
#include "telnetpp/token.hpp"
#include <functional>
#include <string>

namespace telnetpp {
    
//* =========================================================================
/// \brief A structure that can be used to distribute incoming Telnet tokens
/// amongst known routers.
//* =========================================================================
class routing_visitor 
  : public boost::static_visitor<std::vector<token_pass>>
{
public :
    routing_visitor(
        std::function<std::vector<token_pass> (std::string const &)> on_text,
        command_router &cmd_router,
        negotiation_router &neg_router,
        subnegotiation_router &sub_router);
        
    std::vector<token_pass> operator()(std::string const &text);
    std::vector<token_pass> operator()(command const &cmd);
    std::vector<token_pass> operator()(negotiation const &neg);
    std::vector<token_pass> operator()(subnegotiation const &sub);
    
private :
    std::function<std::vector<token_pass> (std::string const &)> on_text_;
    command_router &command_router_;
    negotiation_router &negotiation_router_;
    subnegotiation_router &subnegotiation_router_;
};

}

#endif
