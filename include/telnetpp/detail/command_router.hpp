#pragma once

#include "telnetpp/detail/router.hpp"
#include "telnetpp/command.hpp"
#include "telnetpp/element.hpp"

namespace telnetpp { namespace detail {

struct command_router_key_from_message_policy
{
    static command_type key_from_message(command const &cmd)
    {
        return cmd.value();
    }
};

class command_router
  : public router<
        command_type,
        command,
        void (
            command const &,
            std::function<void (telnetpp::element const &)>),
        detail::command_router_key_from_message_policy
    >
{
};

}}
