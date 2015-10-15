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
auto generate(InputIterator1 begin, InputIterator2 end)
{
    typedef boost::variant<std::vector<u8>, boost::any> result;
    std::vector<result> results;
    std::vector<u8> stream;

    std::for_each(begin, end, [&results, &stream](auto &&token)
    {
        if (token.type() == typeid(element))
        {
            detail::generate_helper(
                stream, 
                boost::get<element>(token));
        }
        else
        {
            if (!stream.empty())
            {
                results.push_back(result(stream));
                stream.clear();
            }
            
            results.push_back(result(boost::get<boost::any>(token)));
        }
    });
    
    if (!stream.empty())
    {
        results.push_back(result(stream));
    }
    
    return results;
}

//* =========================================================================
/// \brief Parse a collection of Telnet tokens into a stream of bytes.
/// \param collection the collection containing the tokens.
//* =========================================================================
template <class Collection>
auto generate(Collection &&collection)
{
    using std::begin;
    using std::end;
    
    return generate(begin(collection), end(collection));
}

}

#endif
