#include "telnetpp/byte_converter.hpp"
#include "telnetpp/detail/lambda_visitor.hpp"

namespace telnetpp {

// ==========================================================================
// SEND
// ==========================================================================
byte_stream byte_converter::send(std::vector<stream_token> const &tokens)
{
    byte_stream result;

    for_each(tokens.begin(), tokens.end(), [&result](auto const &token)
    {
        detail::visit_lambdas(
            token,
            [&result](byte_stream const &data)
            {
                result.insert(result.end(), data.begin(), data.end());
            },
            [](boost::any const &){});
    });

    return result;
}

}
