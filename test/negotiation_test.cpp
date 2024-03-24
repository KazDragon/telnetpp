#include <gtest/gtest.h>
#include <telnetpp/negotiation.hpp>

#include <sstream>
#include <tuple>

using testing::ValuesIn;

using negotiation_string = std::tuple<telnetpp::negotiation, std::string>;

class negotiations_with_strings
  : public testing::TestWithParam<negotiation_string>
{
};

TEST_P(negotiations_with_strings, can_be_streamed_to_an_ostream)
{
    using std::get;

    auto const &param = GetParam();
    auto const &negotiation = get<0>(param);
    auto const &expected_string = get<1>(param);

    std::stringstream stream;
    std::ostream &out = stream;

    out << negotiation;
    ASSERT_EQ(expected_string, stream.str());
}

static negotiation_string const negotiation_strings[] = {
    {telnetpp::negotiation{telnetpp::will, 0x00}, "negotiation[WILL, 0x00]"},
    {telnetpp::negotiation{telnetpp::wont, 0x00}, "negotiation[WONT, 0x00]"},
    {telnetpp::negotiation{telnetpp::do_, 0x00},  "negotiation[DO, 0x00]"  },
    {telnetpp::negotiation{telnetpp::dont, 0x00}, "negotiation[DONT, 0x00]"},

    {telnetpp::negotiation{telnetpp::will, 0x5A}, "negotiation[WILL, 0x5A]"},
    {telnetpp::negotiation{telnetpp::wont, 0xA5}, "negotiation[WONT, 0xA5]"},
    {telnetpp::negotiation{telnetpp::do_, 0x07},  "negotiation[DO, 0x07]"  },
    {telnetpp::negotiation{telnetpp::dont, 0x10}, "negotiation[DONT, 0x10]"},
    {telnetpp::negotiation{telnetpp::will, 0xCC}, "negotiation[WILL, 0xCC]"},
    {telnetpp::negotiation{telnetpp::wont, 0xDD}, "negotiation[WONT, 0xDD]"},
    {telnetpp::negotiation{telnetpp::do_, 0xEE},  "negotiation[DO, 0xEE]"  },
    {telnetpp::negotiation{telnetpp::dont, 0xFF}, "negotiation[DONT, 0xFF]"},
};

INSTANTIATE_TEST_SUITE_P(
    negotiations_can_be_streamed_to_an_ostream,
    negotiations_with_strings,
    ValuesIn(negotiation_strings));
