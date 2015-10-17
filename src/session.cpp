#include "telnetpp/session.hpp"
#include "telnetpp/generator.hpp"
#include "telnetpp/parser.hpp"
#include "telnetpp/detail/registration.hpp"

namespace telnetpp {

// ==========================================================================
// CONSTRUCTOR    
// ==========================================================================
session::session(
    std::function<std::vector<token> (std::string const &)> on_text)
  : visitor_(
        on_text,
        command_router_,
        negotiation_router_,
        subnegotiation_router_)
{
}

void session::install(
    command const &cmd,
    std::function<std::vector<token> (command const &)> const &on_command)
{
    command_router_.register_route(cmd, on_command);
}

// ==========================================================================
// INSTALL
// ==========================================================================
void session::install(client_option &option)
{
    detail::register_client_option(
        option, negotiation_router_, subnegotiation_router_);
}

// ==========================================================================
// INSTALL
// ==========================================================================
void session::install(server_option &option)
{
    detail::register_server_option(
        option, negotiation_router_, subnegotiation_router_);
}

// ==========================================================================
// RECEIVE
// ==========================================================================
std::vector<token> session::receive(const u8stream& stream)
{
    using std::begin;
    using std::end;
    
    unparsed_buffer_.insert(
        end(unparsed_buffer_),
        begin(stream),
        end(stream));
    
    auto it1 = begin(unparsed_buffer_);
    auto it2 = end(unparsed_buffer_);
    
    auto parse_results = parse(it1, it2);

    unparsed_buffer_.erase(begin(unparsed_buffer_), it1);

    std::vector<token> results;
    
    for (auto &&parse_result : parse_results)
    {
        auto result = boost::apply_visitor(visitor_, parse_result);
        
        results.insert(end(results), begin(result), end(result));
    }
    
    return results;
}

// ==========================================================================
// SEND
// ==========================================================================
std::vector<boost::variant<u8stream, boost::any>> session::send(
    std::vector<token> const &tokens)
{
    using std::begin;
    using std::end;
    
    return generate(tokens);
}

}
