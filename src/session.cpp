#include "telnetpp/session.hpp"
#include "telnetpp/detail/command_router.hpp"
#include "telnetpp/detail/negotiation_router.hpp"
#include "telnetpp/detail/overloaded.hpp"
#include "telnetpp/detail/registration.hpp"
#include "telnetpp/detail/subnegotiation_router.hpp"
#include "telnetpp/generator.hpp"
#include "telnetpp/parser.hpp"

namespace telnetpp {

struct session::impl
{
  telnetpp::parser parser_;
  telnetpp::detail::command_router command_router_;
  telnetpp::detail::negotiation_router negotiation_router_;
  telnetpp::detail::subnegotiation_router subnegotiation_router_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
session::session() : pimpl_{std::make_unique<impl>()}
{
  // By default, the session will respond to WILL/WONT(option) with
  // DONT(option), and to DO/DONT(option) with WONT(option).  This behaviour
  // is overridden with any installed options.
  pimpl_->negotiation_router_.set_unregistered_route(
      [this](telnetpp::negotiation const &negotiation)
      {
        auto const &request = negotiation.request();

        negotiation_type result =
            (request == telnetpp::will || request == telnetpp::wont)
                ? telnetpp::dont
                : telnetpp::wont;

        write(telnetpp::negotiation{result, negotiation.option_code()});
      });
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
session::~session() = default;

// ==========================================================================
// IS_ALIVE
// ==========================================================================
bool session::is_alive() const
{
  return channel_->is_alive();
}

// ==========================================================================
// CLOSE
// ==========================================================================
void session::close()
{
  channel_->close();
}

// ==========================================================================
// ASYNC_READ
// ==========================================================================
void session::async_read(std::function<void(telnetpp::bytes)> const &callback)
{
  channel_->async_read(
      [this, callback](telnetpp::bytes content)
      {
        auto const &token_handler =
            [this, callback](telnetpp::element const &elem)
        {
          std::visit(
              detail::overloaded{
                  [&](telnetpp::bytes input_content)
                  { callback(input_content); },
                  [&](telnetpp::command const &cmd)
                  { pimpl_->command_router_(cmd); },
                  [&](telnetpp::negotiation const &neg)
                  { pimpl_->negotiation_router_(neg); },
                  [&](telnetpp::subnegotiation const &sub)
                  { pimpl_->subnegotiation_router_(sub); }},
              elem);
        };

        pimpl_->parser_(content, token_handler);
        callback({});
      });
}

// ==========================================================================
// WRITE
// ==========================================================================
void session::write(telnetpp::element const &elem)
{
  telnetpp::generate(
      elem, [this](telnetpp::bytes data) { channel_->write(data); });
}

// ==========================================================================
// INSTALL
// ==========================================================================
void session::install(
    telnetpp::command_type cmd,
    std::function<void(telnetpp::command)> const &handler)
{
  pimpl_->command_router_.register_route(cmd, handler);
}

// ==========================================================================
// INSTALL
// ==========================================================================
void session::install(client_option &option)
{
  detail::register_client_option(
      option, pimpl_->negotiation_router_, pimpl_->subnegotiation_router_);
}

// ==========================================================================
// INSTALL
// ==========================================================================
void session::install(server_option &option)
{
  detail::register_server_option(
      option, pimpl_->negotiation_router_, pimpl_->subnegotiation_router_);
}

}  // namespace telnetpp
