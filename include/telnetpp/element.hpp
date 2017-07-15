#pragma once

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
/// \class telnetpp::element
/// \brief A common type that can contain any Telnet operation, such as
/// a command, negotiation, or subnegotiation, or a string that represents
/// upper-layer non-Telnet data.
//* =========================================================================
using element = boost::variant<
    std::string,
    negotiation,
    subnegotiation,
    command
>;

//* =========================================================================
/// \class telnetpp::token
/// \brief A type that represents either an element generated from our layer,
/// or a type generated from a different layer.  This can be used to pass
/// elements from other layers through the Telnet layer.
//* =========================================================================
using token = boost::variant<element, boost::any>;

//* =========================================================================
/// \class telnetpp::stream_token
/// \brief A type that represents a token after it has been converted to
/// bytes, or after the object represented by the boost::any member has been
/// passed through.
//* =========================================================================
using stream_token = boost::variant<u8stream, boost::any>;

//* =========================================================================
/// \brief A combiner for tokens.  This can be used during signalling to
/// combine the results of all of the signals.
//* =========================================================================
struct token_combiner
{
    /// The result type of combining the tokens
    using result_type = std::vector<token>;

    /// Combine a range of tokens into a vector.
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
