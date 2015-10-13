#ifndef TELNETPP_ROUTING_VISITOR_HPP_
#define TELNETPP_ROUTING_VISITOR_HPP_

#include "telnetpp/command_router.hpp"
#include "telnetpp/negotiation_router.hpp"
#include "telnetpp/subnegotiation_router.hpp"
#include "telnetpp/element.hpp"
#include <boost/any.hpp>
#include <functional>
#include <string>

namespace telnetpp {
    
//* =========================================================================
/// \brief A structure that can be used to distribute incoming Telnet tokens
/// amongst known routers.
//* =========================================================================
class routing_visitor 
  : public boost::static_visitor<std::vector<token>>
{
public :
    routing_visitor(
        std::function<std::vector<token> (std::string const &)> on_text,
        std::function<std::vector<token> (boost::any const &)> on_any,
        command_router &cmd_router,
        negotiation_router &neg_router,
        subnegotiation_router &sub_router);
        
    std::vector<token> operator()(std::string const &text);
    std::vector<token> operator()(boost::any const &any);
    std::vector<token> operator()(command const &cmd);
    std::vector<token> operator()(negotiation const &neg);
    std::vector<token> operator()(subnegotiation const &sub);
    
private :
    std::function<std::vector<token> (std::string const &)> on_text_;
    std::function<std::vector<token> (boost::any const &)> on_any_;
    command_router &command_router_;
    negotiation_router &negotiation_router_;
    subnegotiation_router &subnegotiation_router_;
};

}

#endif
