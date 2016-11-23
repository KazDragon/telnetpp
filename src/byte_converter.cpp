#include "telnetpp/byte_converter.hpp"
#include "telnetpp/detail/lambda_visitor.hpp"

namespace telnetpp {

u8stream byte_converter::send(std::vector<stream_token> const &tokens)
{
    u8stream result;

    for_each(tokens.begin(), tokens.end(), [&result](auto const &token)
    {
        boost::apply_visitor(
            detail::make_lambda_visitor<void>(
                [&result](u8stream const &data)
                {
                    result.insert(result.end(), data.begin(), data.end());
                },
                [](boost::any const &)
                {
                }),
            token);
    });

    return result;
}

}
