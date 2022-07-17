#pragma once

#include "telnetpp/element.hpp"
#include "telnetpp/detail/generate_helper.hpp"
#include "telnetpp/detail/overloaded.hpp"
#include <algorithm>
#include <utility>

namespace telnetpp {

//* =========================================================================
/// \brief Transform a Telnet element into streams of bytes.
/// \param elem the elements to transform.
/// \param cont a continuation that will be called with a number
///        of spans of bytes, which will represent the transformed
///        data.
//* =========================================================================
template <class Continuation>
constexpr void generate(telnetpp::element const &elem, Continuation &&cont)
{
    std::visit(detail::overloaded{
        [&cont](telnetpp::bytes data)
        {
            detail::generate_escaped(data, cont);
        },
        [&cont](telnetpp::command const &cmd)
        {
            detail::generate_command(cmd, cont);
        },
        [&cont](telnetpp::negotiation const &neg)
        {
            detail::generate_negotiation(neg, cont);
        },
        [&cont](telnetpp::subnegotiation const &sub)
        {
            detail::generate_subnegotiation(sub, cont);
        }},
        elem);
}

}
