#pragma once

#include "telnetpp/detail/generate_helper.hpp"
#include "telnetpp/detail/lambda_visitor.hpp"
#include <algorithm>
#include <iterator>
#include <utility>

namespace telnetpp { namespace detail {

//* =========================================================================
/// \brief Parse a series of Telnet tokens into a stream of bytes.
/// \param begin an iterator to the start of the series.
/// \param end an iterator to one past the end of the series.
//* =========================================================================
template <class InputIterator1, class InputIterator2>
auto generate(InputIterator1 begin, InputIterator2 end)
{
    std::vector<stream_token> results;
    u8stream stream;

    std::for_each(begin, end, [&results, &stream](auto &&token)
    {
        boost::apply_visitor(
            make_lambda_visitor<void>(
               [&stream](element const &elem) 
               {
                    detail::generate_helper(stream, elem);
               },
               [&stream, &results](boost::any const &any)
               {
                    if (!stream.empty())
                    {
                        results.push_back(stream);
                        stream.clear();
                    }
        
                    results.push_back(any);
               }),
            token);
    });

    if (!stream.empty())
    {
        results.push_back(stream);
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

}}
