#pragma once

#include "telnetpp/detail/router.hpp"
#include "telnetpp/subnegotiation.hpp"
#include "telnetpp/element.hpp"
#include <vector>

namespace telnetpp { namespace detail {

struct subnegotiation_router_key_from_message_policy
{
    static telnetpp::u8 key_from_message(subnegotiation const &sub)
    {
        return sub.option();
    }
};

class subnegotiation_router
  : public router<
        u8,
        subnegotiation,
        std::vector<telnetpp::token>,
        detail::subnegotiation_router_key_from_message_policy
    >
{
};

}}
