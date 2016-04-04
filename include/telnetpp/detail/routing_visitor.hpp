#pragma once

#include "telnetpp/detail/command_router.hpp"
#include "telnetpp/detail/negotiation_router.hpp"
#include "telnetpp/detail/subnegotiation_router.hpp"
#include "telnetpp/element.hpp"
#include <functional>
#include <string>

namespace telnetpp { namespace detail {

//* =========================================================================
/// \brief A structure that can be used to distribute incoming Telnet tokens
/// amongst known routers.
//* =========================================================================
class routing_visitor
  : public boost::static_visitor<std::vector<telnetpp::token>>
{
public :
    routing_visitor(
        std::function<std::vector<telnetpp::token> (std::string const &)> on_text,
        command_router &cmd_router,
        negotiation_router &neg_router,
        subnegotiation_router &sub_router);

    std::vector<telnetpp::token> operator()(std::string const &text);
    std::vector<telnetpp::token> operator()(command const &cmd);
    std::vector<telnetpp::token> operator()(negotiation const &neg);
    std::vector<telnetpp::token> operator()(subnegotiation const &sub);

private :
    std::function<std::vector<telnetpp::token> (std::string const &)> on_text_;
    command_router &command_router_;
    negotiation_router &negotiation_router_;
    subnegotiation_router &subnegotiation_router_;
};

}}
