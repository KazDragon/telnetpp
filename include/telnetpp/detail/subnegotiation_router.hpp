#pragma once

#include "telnetpp/detail/router.hpp"
#include "telnetpp/element.hpp"
#include "telnetpp/subnegotiation.hpp"
#include <functional>

namespace telnetpp::detail {

struct subnegotiation_router_key_from_message_policy
{
  static option_type key_from_message(subnegotiation const &sub)
  {
    return sub.option();
  }
};

class subnegotiation_router
  : public router<
        option_type,
        subnegotiation,
        void(telnetpp::subnegotiation),
        detail::subnegotiation_router_key_from_message_policy>
{
};

}  // namespace telnetpp::detail
