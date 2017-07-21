#include "telnetpp/session.hpp"
#include "telnetpp/detail/generator.hpp"
#include "telnetpp/detail/parser.hpp"
#include "telnetpp/detail/registration.hpp"
#include <numeric>

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
    // By default, the session will respond to WILL/WONT(option) with
    // DONT(option), and to DO/DONT(option) with WONT(option).  This behaviour
    // is overridden with any installed options.
    negotiation_router_.set_unregistered_route(
        [](telnetpp::negotiation const &negotiation)
            -> std::vector<telnetpp::token>
        {
            auto const &request = negotiation.request();

            negotiation_type result =
                (request == telnetpp::will || request == telnetpp::wont)
              ? telnetpp::dont
              : telnetpp::wont;

            return {
                telnetpp::element {
                    telnetpp::negotiation(result, negotiation.option())
                }
            };
        });
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
std::vector<token> session::receive(const byte_stream& stream)
{
    using std::begin;
    using std::end;

    unparsed_buffer_.insert(
        end(unparsed_buffer_),
        begin(stream),
        end(stream));

    auto it1 = begin(unparsed_buffer_);
    auto it2 = end(unparsed_buffer_);

    auto const &parse_results = telnetpp::detail::parse(it1, it2);

    unparsed_buffer_.erase(begin(unparsed_buffer_), it1);

    auto const &results = std::accumulate(
        parse_results.begin(),
        parse_results.end(),
        std::vector<token>{},
        [this](auto &results, auto const &parse_result)
        {
            auto const &result = boost::apply_visitor(visitor_, parse_result);
            results.insert(end(results), begin(result), end(result));
            return results;
        });

    return results;
}

// ==========================================================================
// SEND
// ==========================================================================
std::vector<boost::variant<byte_stream, boost::any>> session::send(
    std::vector<token> const &tokens)
{
    return detail::generate(tokens);
}

}
