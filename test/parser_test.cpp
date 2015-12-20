#include "telnetpp/detail/parser.hpp"
#include "expect_elements.hpp"
#include <gtest/gtest.h>

TEST(parser_test, empty_range_parses_to_nothing)
{
    auto data = telnetpp::u8stream{};
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::detail::parse(begin, end);
    
    ASSERT_EQ(size_t{0}, result.size());
    ASSERT_TRUE(begin == end);
}

TEST(parser_test, normal_character_parses_to_string)
{
    auto data = telnetpp::u8stream{ 'x' };
    std::string expected = "x";

    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::detail::parse(begin, end);
    
    ASSERT_EQ(size_t{1}, result.size());
    ASSERT_TRUE(begin == end);
    
    std::string actual = boost::get<std::string>(result[0]);
    
    ASSERT_EQ(expected, actual);
}

TEST(parser_test, two_normal_characters_parse_to_string)
{
    auto data = telnetpp::u8stream{ 'x', 'y' };
    std::string expected = "xy";

    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::detail::parse(begin, end);
    
    ASSERT_EQ(size_t{1}, result.size());
    ASSERT_TRUE(begin == end);
    
    std::string actual = boost::get<std::string>(result[0]);
    
    ASSERT_EQ(expected, actual);
}

TEST(parser_test, lone_iac_parses_to_nothing)
{
    auto data = telnetpp::u8stream{ 0xFF };
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::detail::parse(begin, end);
    ASSERT_EQ(size_t{0}, result.size());
    ASSERT_TRUE(begin == data.begin());
}

TEST(parser_test, double_iac_parses_to_iac)
{
    auto data = telnetpp::u8stream{ 0xFF, 0xFF };
    auto expected = std::string("\xFF");
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::detail::parse(begin, end);
    ASSERT_EQ(size_t{1}, result.size());
    ASSERT_EQ(expected, boost::get<std::string>(result[0]));
    ASSERT_TRUE(begin == end);
}

TEST(parser_test, iac_nop_parses_to_command)
{
    auto data = telnetpp::u8stream{ 0xFF, 0xF1 };
    auto expected = telnetpp::command(telnetpp::nop);
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::detail::parse(begin, end);
    ASSERT_EQ(size_t{1}, result.size());
    ASSERT_EQ(expected, boost::get<telnetpp::command>(result[0]));
    ASSERT_TRUE(begin == end);
}

TEST(parser_test, iac_dm_parses_to_command)
{
    auto data = telnetpp::u8stream{ 0xFF, 0xF2 };
    auto expected = telnetpp::command(telnetpp::dm);
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::detail::parse(begin, end);
    ASSERT_EQ(size_t{1}, result.size());
    ASSERT_EQ(expected, boost::get<telnetpp::command>(result[0]));
    ASSERT_TRUE(begin == end);
}

TEST(parser_test, iac_brk_parses_to_command)
{
    auto data = telnetpp::u8stream{ 0xFF, 0xF3 };
    auto expected = telnetpp::command(telnetpp::brk);
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::detail::parse(begin, end);
    ASSERT_EQ(size_t{1}, result.size());
    ASSERT_EQ(expected, boost::get<telnetpp::command>(result[0]));
    ASSERT_TRUE(begin == end);
}

TEST(parser_test, iac_ip_parses_to_command)
{
    auto data = telnetpp::u8stream{ 0xFF, 0xF4 };
    auto expected = telnetpp::command(telnetpp::ip);
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::detail::parse(begin, end);
    ASSERT_EQ(size_t{1}, result.size());
    ASSERT_EQ(expected, boost::get<telnetpp::command>(result[0]));
    ASSERT_TRUE(begin == end);
}

TEST(parser_test, iac_ao_parses_to_command)
{
    auto data = telnetpp::u8stream{ 0xFF, 0xF5 };
    auto expected = telnetpp::command(telnetpp::ao);
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::detail::parse(begin, end);
    ASSERT_EQ(size_t{1}, result.size());
    ASSERT_EQ(expected, boost::get<telnetpp::command>(result[0]));
    ASSERT_TRUE(begin == end);
}

TEST(parser_test, iac_ayt_parses_to_command)
{
    auto data = telnetpp::u8stream{ 0xFF, 0xF6 };
    auto expected = telnetpp::command(telnetpp::ayt);
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::detail::parse(begin, end);
    ASSERT_EQ(size_t{1}, result.size());
    ASSERT_EQ(expected, boost::get<telnetpp::command>(result[0]));
    ASSERT_TRUE(begin == end);
}

TEST(parser_test, iac_ec_parses_to_command)
{
    auto data = telnetpp::u8stream{ 0xFF, 0xF7 };
    auto expected = telnetpp::command(telnetpp::ec);
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::detail::parse(begin, end);
    ASSERT_EQ(size_t{1}, result.size());
    ASSERT_EQ(expected, boost::get<telnetpp::command>(result[0]));
    ASSERT_TRUE(begin == end);
}

TEST(parser_test, iac_el_parses_to_command)
{
    auto data = telnetpp::u8stream{ 0xFF, 0xF8 };
    auto expected = telnetpp::command(telnetpp::el);
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::detail::parse(begin, end);
    ASSERT_EQ(size_t{1}, result.size());
    ASSERT_EQ(expected, boost::get<telnetpp::command>(result[0]));
    ASSERT_TRUE(begin == end);
}

TEST(parser_test, iac_ga_parses_to_command)
{
    auto data = telnetpp::u8stream{ 0xFF, 0xF9 };
    auto expected = telnetpp::command(telnetpp::ga);
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::detail::parse(begin, end);
    ASSERT_EQ(size_t{1}, result.size());
    ASSERT_EQ(expected, boost::get<telnetpp::command>(result[0]));
    ASSERT_TRUE(begin == end);
}

TEST(parser_test, iac_sb_parses_to_nothing)
{
    auto data = telnetpp::u8stream{ 0xFF, 0xFA };
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::detail::parse(begin, end);
    ASSERT_EQ(size_t{0}, result.size());
    ASSERT_TRUE(begin == data.begin());
}

TEST(parser_test, iac_will_parses_to_nothing)
{
    auto data = telnetpp::u8stream{ 0xFF, 0xFB };
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::detail::parse(begin, end);
    ASSERT_EQ(size_t{0}, result.size());
    ASSERT_TRUE(begin == data.begin());
}

TEST(parser_test, iac_wont_parses_to_nothing)
{
    auto data = telnetpp::u8stream{ 0xFF, 0xFC };
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::detail::parse(begin, end);
    ASSERT_EQ(size_t{0}, result.size());
    ASSERT_TRUE(begin == data.begin());
}

TEST(parser_test, iac_do_parses_to_nothing)
{
    auto data = telnetpp::u8stream{ 0xFF, 0xFD };
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::detail::parse(begin, end);
    ASSERT_EQ(size_t{0}, result.size());
    ASSERT_TRUE(begin == data.begin());

}

TEST(parser_test, iac_dont_parses_to_nothing)
{
    auto data = telnetpp::u8stream{ 0xFF, 0xFE };
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::detail::parse(begin, end);
    ASSERT_EQ(size_t{0}, result.size());
    ASSERT_TRUE(begin == data.begin());
}

TEST(parser_test, will_negotiation_parses_to_negotiation)
{
    auto data = telnetpp::u8stream{ 0xFF, 0xFB, 0xAB };
    auto expected = telnetpp::negotiation(telnetpp::will, 0xAB);
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::detail::parse(begin, end);
    ASSERT_EQ(size_t{1}, result.size());
    ASSERT_EQ(
        expected, boost::get<telnetpp::negotiation>(result[0]));
    ASSERT_TRUE(begin == end);
}

TEST(parser_test, wont_negotiation_parses_to_negotiation)
{
    auto data = telnetpp::u8stream{ 0xFF, 0xFC, 0xBC };
    auto expected = telnetpp::negotiation(telnetpp::wont, 0xBC);
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::detail::parse(begin, end);
    ASSERT_EQ(size_t{1}, result.size());
    ASSERT_EQ(
        expected, boost::get<telnetpp::negotiation>(result[0]));
    ASSERT_TRUE(begin == end);
}

TEST(parser_test, do_negotiation_parses_to_negotiation)
{
    auto data = telnetpp::u8stream{ 0xFF, 0xFD, 0xCD };
    auto expected = telnetpp::negotiation(telnetpp::do_, 0xCD);
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::detail::parse(begin, end);
    ASSERT_EQ(size_t{1}, result.size());
    ASSERT_EQ(
        expected, boost::get<telnetpp::negotiation>(result[0]));
    ASSERT_TRUE(begin == end);
}

TEST(parser_test, dont_negotiation_parses_to_negotiation)
{
    auto data = telnetpp::u8stream{ 0xFF, 0xFE, 0xDE };
    auto expected = telnetpp::negotiation(telnetpp::dont, 0xDE);
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::detail::parse(begin, end);
    ASSERT_EQ(size_t{1}, result.size());
    ASSERT_EQ(
        expected, boost::get<telnetpp::negotiation>(result[0]));
    ASSERT_TRUE(begin == end);
}

TEST(parser_test, sb_option_parses_to_nothing)
{
    auto data = telnetpp::u8stream{ 0xFF, 0xFA, 0xAA };
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::detail::parse(begin, end);
    ASSERT_EQ(size_t{0}, result.size());
    ASSERT_TRUE(begin == data.begin());
}

TEST(parser_test, sb_option_iac_parses_to_nothing)
{
    auto data = telnetpp::u8stream{ 0xFF, 0xFA, 0xAA, 0xFF };
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::detail::parse(begin, end);
    ASSERT_EQ(size_t{0}, result.size());
    ASSERT_TRUE(begin == data.begin());
}

TEST(parser_test, sb_option_iac_se_parses_to_empty_subnegotiation)
{
    auto data = telnetpp::u8stream{ 
        0xFF, 0xFA, 0xAA, 0xFF, 0xF0 
    };
    auto expected = telnetpp::subnegotiation(0xAA, {});
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::detail::parse(begin, end);
    ASSERT_EQ(size_t{1}, result.size());
    ASSERT_EQ(
        expected, 
        boost::get<telnetpp::subnegotiation>(result[0]));
    ASSERT_TRUE(begin == end);
}

TEST(parser_test, subnegotiation_with_content_parses_to_subnegotation)
{
    auto data = telnetpp::u8stream{
        0xFF, 0xFA, 0xAB, 0x01, 0x02, 0x03, 0xFF, 0xF0
    };
    auto expected = telnetpp::subnegotiation(
        0xAB, { 0x01, 0x02, 0x03 });
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::detail::parse(begin, end);
    ASSERT_EQ(size_t{1}, result.size());
    ASSERT_EQ(
        expected, 
        boost::get<telnetpp::subnegotiation>(result[0]));
    ASSERT_TRUE(begin == end);
}

TEST(parser_test, subnegotiation_with_iac_iac_parses_to_iac)
{
    auto data = telnetpp::u8stream{
        0xFF, 0xFA, 0xAC, 0x01, 0xFF, 0xFF, 0x02, 0x03, 0xFF, 0xF0
    };
    auto expected = telnetpp::subnegotiation(
        0xAC, { 0x01, 0xFF, 0x02, 0x03 });
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::detail::parse(begin, end);
    ASSERT_EQ(size_t{1}, result.size());
    ASSERT_EQ(
        expected, 
        boost::get<telnetpp::subnegotiation>(result[0]));
    ASSERT_TRUE(begin == end);
}

TEST(parser_test, many_elements_parses_to_many_elements)
{
    telnetpp::u8 data[] = {
        'a', 'b', 'c', 0xFF, 0xFF, // string: abc\xFF\xFF
        0xFF, 0xF1,                // command: NOP
        0xFF, 0xF6,                // command: AYT
        'd',                       // string: d
        0xFF, 0xFB, 0x00,          // negotiation: WILL 0
        0xFF, 0xFE, 0x01,          // negotiation: DONT 1
        0xFF, 0xFA, 0x20, 'e', 'f', 0xFF, 0xF0,
                                   // subnegotiation: 0x20[e,f]
        0xFF, 0xFA, 0xFF, 0xFF, 0xF0,
                                   // subnegotiation: 0xFF[]
        'g', 'h',                  // string: gh
        0xFF, 0xFC, 0xFF,          // negotiation: WONT 0xFF
        0xFF, 0xFF, 0xFF, 0xFF, 'i'// text: \xFF\xFFi
    };
    
    auto begin = std::begin(data);
    auto end   = std::end(data);
    
    expect_elements(
        {
            std::string("abc\xFF"),
            telnetpp::command(telnetpp::nop),
            telnetpp::command(telnetpp::ayt),
            std::string("d"),
            telnetpp::negotiation(telnetpp::will, 0x00),
            telnetpp::negotiation(telnetpp::dont, 0x01),
            telnetpp::subnegotiation(0x20, {'e', 'f'}),
            telnetpp::subnegotiation(0xFF, {}),
            std::string("gh"),
            telnetpp::negotiation(telnetpp::wont, 0xFF),
            std::string("\xFF\xFFi") 
        },
        telnetpp::detail::parse(begin, end));
    
    ASSERT_TRUE(begin == end);
}
