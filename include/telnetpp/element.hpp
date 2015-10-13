#ifndef TELNETPP_ELEMENT_HPP_
#define TELNETPP_ELEMENT_HPP_

#include "telnetpp/command.hpp"
#include "telnetpp/negotiation.hpp"
#include "telnetpp/subnegotiation.hpp"
#include <boost/any.hpp>
#include <boost/variant.hpp>
#include <numeric>
#include <string>
#include <vector>

namespace telnetpp {
    
//* =========================================================================
/// \brief A common type that can contain any Telnet operation.
//* =========================================================================
typedef boost::variant<
    std::string,
    negotiation,
    subnegotiation,
    command
> element;

//* =========================================================================
/// \brief A type that represents either an element generated from our layer,
/// or a type generated from a different layer.  This can be used to pass
/// elements from other layers through the Telnet layer.
//* =========================================================================
typedef boost::variant<element, boost::any> token;

//* =========================================================================
/// \brief A combiner for tokens.  This can be used during signalling to
/// combine the results of all of the signals.
//* =========================================================================
struct token_combiner
{
    typedef std::vector<token> result_type;
    
    template <class InputIterator1, class InputIterator2>
    std::vector<token> operator()(InputIterator1 begin, InputIterator2 end) const
    {
        return std::accumulate(
            begin, 
            end,
            std::vector<token>{},
            [](auto &&lhs, auto &&rhs)
            {
                lhs.insert(lhs.end(), rhs.begin(), rhs.end());
                return lhs;
            });
    }
};

}

#endif
