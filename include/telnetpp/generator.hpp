#ifndef TELNETPP_GENERATOR_HPP_
#define TELNETPP_GENERATOR_HPP_

#include "telnetpp/detail/generate_helper.hpp"
#include <algorithm>
#include <iterator>
#include <utility>

namespace telnetpp {
 
//* =========================================================================
/// \brief Parse a series of Telnet tokens into a stream of bytes.
/// \param begin an iterator to the start of the series.
/// \param end an iterator to one past the end of the series.
//* =========================================================================
template <class InputIterator1, class InputIterator2>
std::vector<u8> generate(InputIterator1 begin, InputIterator2 end)
{
    std::vector<u8> result;
    
    std::for_each(begin, end, [&result](auto &&token)
    {
        detail::generate_helper(result, token);
    });
    
    return result;
}

//* =========================================================================
/// \brief Parse a collection of Telnet tokens into a stream of bytes.
/// \param collection the collection containing the tokens.
//* =========================================================================
template <class Collection>
std::vector<u8> generate(Collection &&collection)
{
    using std::begin;
    using std::end;
    
    return generate(begin(collection), end(collection));
}

}

#endif
