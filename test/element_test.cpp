#include <gtest/gtest.h>
#include <telnetpp/element.hpp>

#include <sstream>
#include <tuple>

using testing::ValuesIn;

using element_string = std::tuple<telnetpp::element, std::string>;

class elements_with_strings : public testing::TestWithParam<element_string>
{
};

TEST_P(elements_with_strings, can_be_streamed_to_an_ostream)
{
    using std::get;

    auto const &param = GetParam();
    auto const &element = get<0>(param);
    auto const &expected_string = get<1>(param);

    std::stringstream stream;
    std::ostream &out = stream;

    out << element;
    ASSERT_EQ(expected_string, stream.str());
}

static telnetpp::byte_storage const subnegotiation0_content{};
static telnetpp::byte_storage const subnegotiation1_content{0x10, 0x80, 0xC0};

static element_string const element_strings[] = {
    {telnetpp::element{telnetpp::command{telnetpp::ayt}},
     "element[command[AYT]]"                            },
    {telnetpp::element{telnetpp::negotiation{telnetpp::will, 0xDC}},
     "element[negotiation[WILL, 0xDC]]"                 },
    {telnetpp::element{telnetpp::subnegotiation{0x00, subnegotiation0_content}},
     "element[subnegotiation[0x00, []]]"                },
    {telnetpp::element{telnetpp::subnegotiation{0x01, subnegotiation1_content}},
     "element[subnegotiation[0x01, [0x10, 0x80, 0xC0]]]"},
};

INSTANTIATE_TEST_SUITE_P(
    elements_can_be_streamed_to_an_ostream,
    elements_with_strings,
    ValuesIn(element_strings));
