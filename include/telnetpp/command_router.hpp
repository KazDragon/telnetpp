#ifndef TELNETPP_COMMAND_ROUTER_HPP_
#define TELNETPP_COMMAND_ROUTER_HPP_

#include "telnetpp/router.hpp"
#include "telnetpp/command.hpp"
#include "telnetpp/token.hpp"

namespace telnetpp {
    
namespace detail {
    
struct command_router_key_from_message_policy
{
    static command key_from_message(command const &cmd)
    {
        return cmd;
    }
};

}

class command_router
  : public router<
        command,
        command,
        std::vector<token_pass>,
        detail::command_router_key_from_message_policy
    >
{
};

}


#endif
