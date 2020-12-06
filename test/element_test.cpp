#include <telnetpp/element.hpp>
#include <gtest/gtest.h>
#include <tuple>
#include <sstream>

using testing::ValuesIn;

using element_string = std::tuple<
    telnetpp::element,
    std::string
>;

class elements_with_strings
  : public testing::TestWithParam<element_string>
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

static const telnetpp::byte_storage subnegotiation0_content {};
static const telnetpp::byte_storage subnegotiation1_content {0x10, 0x80, 0xC0};

static element_string const element_strings[] = {
    element_string{ telnetpp::element{telnetpp::command{telnetpp::ayt}}, "element[command[AYT]]" },
    element_string{ telnetpp::element{telnetpp::negotiation{telnetpp::will, 0xDC}}, "element[negotiation[WILL, 0xDC]]" },
    element_string{ telnetpp::element{telnetpp::subnegotiation{0x00, subnegotiation0_content}}, "element[subnegotiation[0x00, []]]" },
    element_string{ telnetpp::element{telnetpp::subnegotiation{0x01, subnegotiation1_content}}, "element[subnegotiation[0x01, [0x10, 0x80, 0xC0]]]" },
};

INSTANTIATE_TEST_CASE_P(
    elements_can_be_streamed_to_an_ostream,
    elements_with_strings,
    ValuesIn(element_strings)
);
