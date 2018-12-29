#pragma once

#include "telnetpp/element.hpp"
#include "telnetpp/detail/generate_helper.hpp"
#include "telnetpp/detail/lambda_visitor.hpp"
#include <algorithm>
#include <utility>

namespace telnetpp {

//* =========================================================================
/// \brief Transform a collection of Telnet elements into streams of bytes.
/// \param collection the collection of elements to transform.
/// \param cont a continuation that will be called with a number
///        of spans of bytes, which will represent the transformed
///        data.
//* =========================================================================
template <class Collection, class Continuation>
constexpr void generate(Collection &&collection, Continuation &&cont)
{
    using std::begin;
    using std::end;

    std::for_each(begin(collection), end(collection), 
        [&cont](telnetpp::element const &elem)
        {
            detail::visit_lambdas(
                elem,
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
                }
            );
        });
}

}
