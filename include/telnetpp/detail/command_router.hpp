#pragma once

#include "telnetpp/command.hpp"
#include "telnetpp/detail/router.hpp"

namespace telnetpp::detail {

struct command_router_key_from_message_policy
{
    static command_type key_from_message(command const &cmd)
    {
        return cmd.value();
    }
};

class command_router : public router<
                           command_type,
                           command,
                           void(command),
                           detail::command_router_key_from_message_policy>
{
};

}  // namespace telnetpp::detail
