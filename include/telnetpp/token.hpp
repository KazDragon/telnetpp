#ifndef TELNETPP_TOKEN_HPP_
#define TELNETPP_TOKEN_HPP_

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
/// \brief A common token type for all Telnet operations.
//* =========================================================================
typedef boost::variant<
    std::string,
    negotiation,
    subnegotiation,
    command
> token;

//* =========================================================================
/// \brief A type that represents either a token generated from our layer,
/// or a type generated from a different layer
//* =========================================================================
typedef boost::variant<token, boost::any> token_pass;

//* =========================================================================
/// \brief A combiner for tokens.  This can be used during signalling to
/// combine the results of all of the signals.
//* =========================================================================
template <class T>
struct token_combiner
{
    typedef std::vector<T> result_type;
    
    template <class InputIterator1, class InputIterator2>
    std::vector<T> operator()(InputIterator1 begin, InputIterator2 end) const
    {
        return std::accumulate(
            begin, 
            end,
            std::vector<T>{},
            [](auto &&lhs, auto &&rhs)
            {
                lhs.insert(lhs.end(), rhs.begin(), rhs.end());
                return lhs;
            });
    }
};

}

#endif
