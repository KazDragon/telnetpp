#pragma once

#include "telnetpp/detail/router.hpp"
#include "telnetpp/element.hpp"
#include "telnetpp/negotiation.hpp"

namespace telnetpp { namespace detail {

struct negotiation_router_key_from_message_policy
{
    static negotiation key_from_message(negotiation const &neg)
    {
        return neg;
    }
};

//* =========================================================================
/// \brief A structure that can route incoming negotiations to their
/// associated options.
///
/// This can be used as a multiplexer between incoming
/// negotiations and the set of supported client and server options.
/// =========================================================================
class negotiation_router
  : public router<
        negotiation,
        negotiation,
        void (
            telnetpp::negotiation, 
            std::function<void (telnetpp::element const &)>),
        detail::negotiation_router_key_from_message_policy
    >
{
};

}}
