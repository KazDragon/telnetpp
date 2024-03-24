#include <gtest/gtest.h>
#include <telnetpp/subnegotiation.hpp>

#include <sstream>
#include <tuple>

using testing::ValuesIn;

using subnegotiation_string = std::tuple<
    telnetpp::byte,          // option
    telnetpp::byte_storage,  // data
    std::string              // expected output
    >;

class subnegotiations_with_strings
  : public testing::TestWithParam<subnegotiation_string>
{
};

TEST_P(subnegotiations_with_strings, can_be_streamed_to_an_ostream)
{
    using std::get;

    auto const &param = GetParam();
    auto const &option = get<0>(param);
    auto const &data = get<1>(param);
    auto const &expected_string = get<2>(param);

    std::stringstream stream;
    std::ostream &out = stream;

    telnetpp::subnegotiation subnegotiation(option, data);
    out << subnegotiation;
    ASSERT_EQ(expected_string, stream.str());
}

static subnegotiation_string const subnegotiation_strings[] = {
    subnegotiation_string{0x00, {},           "subnegotiation[0x00, []]"          },
    subnegotiation_string{0x07, {},           "subnegotiation[0x07, []]"          },
    subnegotiation_string{0xAC, {},           "subnegotiation[0xAC, []]"          },

    subnegotiation_string{0x00, {0x00},       "subnegotiation[0x00, [0x00]]"      },
    subnegotiation_string{0x00, {0x07},       "subnegotiation[0x00, [0x07]]"      },
    subnegotiation_string{0x00, {0xAC},       "subnegotiation[0x00, [0xAC]]"      },

    subnegotiation_string{
                          0x00, {0x00, 0x01}, "subnegotiation[0x00, [0x00, 0x01]]"},
    subnegotiation_string{
                          0x00, {0x07, 0x00}, "subnegotiation[0x00, [0x07, 0x00]]"},
    subnegotiation_string{
                          0x00, {0xAC, 0xDE}, "subnegotiation[0x00, [0xAC, 0xDE]]"},
};

INSTANTIATE_TEST_SUITE_P(
    subnegotiations_can_be_streamed_to_an_ostream,
    subnegotiations_with_strings,
    ValuesIn(subnegotiation_strings));
