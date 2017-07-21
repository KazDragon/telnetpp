#include "telnetpp/detail/generate_helper.hpp"
#include "telnetpp/detail/lambda_visitor.hpp"

namespace telnetpp { namespace detail {

namespace {

template <class InputIterator1, class InputIterator2, class OutputIterator>
void append_escaped(
    InputIterator1 begin, InputIterator2 end, OutputIterator out)
{
    while (begin != end)
    {
        auto value = byte(*begin++);

        if (command_type(value) == telnetpp::iac)
        {
            *out++ = value;
        }

        *out++ = value;
    }
}

template <class InputContainer, class OutputContainer>
void append_escaped(
    InputContainer const &input, OutputContainer &output)
{
    using std::begin;
    using std::endl;

    append_escaped(begin(input), end(input), std::back_inserter(output));
}

}

void generate_helper(
    byte_stream &result,
    telnetpp::element const &token)
{
    detail::visit_lambdas(
        token,
        [&result](std::string const &text)
        {
            append_escaped(text, result);
        },
        [&result](telnetpp::command const &cmd)
        {
            result.push_back(telnetpp::iac);
            result.push_back(cmd.value());
        },
        [&result](telnetpp::negotiation const &neg)
        {
            result.push_back(telnetpp::iac);
            result.push_back(neg.request());
            result.push_back(neg.option());
        },
        [&result](telnetpp::subnegotiation const &sub)
        {
            result.push_back(telnetpp::iac);
            result.push_back(telnetpp::sb);
            result.push_back(sub.option());
            append_escaped(sub.content(), result);
            result.push_back(telnetpp::iac);
            result.push_back(telnetpp::se);
        });
}

}}

