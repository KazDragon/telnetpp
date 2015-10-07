#include "telnetpp/routing_visitor.hpp"

namespace telnetpp {
    
// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
routing_visitor::routing_visitor(
    std::function<std::vector<token> (std::string const &)> on_text,
    command_router &cmd_router,
    negotiation_router &neg_router,
    subnegotiation_router &sub_router)
  : on_text_(std::move(on_text)),
    command_router_(cmd_router),
    negotiation_router_(neg_router),
    subnegotiation_router_(sub_router)
{
}

// ==========================================================================
// OPERATOR()
// ==========================================================================
std::vector<token> routing_visitor::operator()(std::string const &text)
{
    if (on_text_)
    {
        return on_text_(text);
    }
    else
    {
        return {};
    }
}

// ==========================================================================
// OPERATOR()
// ==========================================================================
std::vector<token> routing_visitor::operator()(command const &cmd)
{
    return command_router_(cmd);
}

// ==========================================================================
// OPERATOR()
// ==========================================================================
std::vector<token> routing_visitor::operator()(negotiation const &neg)
{
    return negotiation_router_(neg);
}

// ==========================================================================
// OPERATOR()
// ==========================================================================
std::vector<token> routing_visitor::operator()(subnegotiation const &sub)
{
    subnegotiation_router_(sub);
    return {};
}

}
