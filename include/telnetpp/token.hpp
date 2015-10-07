#ifndef TELNETPP_TOKEN_HPP_
#define TELNETPP_TOKEN_HPP_

#include "telnetpp/command.hpp"
#include "telnetpp/negotiation.hpp"
#include "telnetpp/subnegotiation.hpp"
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
