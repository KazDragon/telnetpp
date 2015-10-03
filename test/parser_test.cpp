#include "telnetpp/parser.hpp"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <vector>

class parser_test : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(parser_test);
        CPPUNIT_TEST(empty_range_parses_to_nothing);
        CPPUNIT_TEST(normal_character_parses_to_string);
        CPPUNIT_TEST(two_normal_characters_parse_to_string);
        CPPUNIT_TEST(lone_iac_parses_to_nothing);
        CPPUNIT_TEST(double_iac_parses_to_iac);
        CPPUNIT_TEST(iac_nop_parses_to_command);
        CPPUNIT_TEST(iac_dm_parses_to_command);
        CPPUNIT_TEST(iac_brk_parses_to_command);
        CPPUNIT_TEST(iac_ip_parses_to_command);
        CPPUNIT_TEST(iac_ao_parses_to_command);
        CPPUNIT_TEST(iac_ayt_parses_to_command);
        CPPUNIT_TEST(iac_ec_parses_to_command);
        CPPUNIT_TEST(iac_el_parses_to_command);
        CPPUNIT_TEST(iac_ga_parses_to_command);
    CPPUNIT_TEST_SUITE_END();
private :
    
    void empty_range_parses_to_nothing();
    void normal_character_parses_to_string();
    void two_normal_characters_parse_to_string();
    void lone_iac_parses_to_nothing();
    void double_iac_parses_to_iac();
    void iac_nop_parses_to_command();
    void iac_dm_parses_to_command();
    void iac_brk_parses_to_command();
    void iac_ip_parses_to_command();
    void iac_ao_parses_to_command();
    void iac_ayt_parses_to_command();
    void iac_ec_parses_to_command();
    void iac_el_parses_to_command();
    void iac_ga_parses_to_command();
    
};

CPPUNIT_TEST_SUITE_REGISTRATION(parser_test);

void parser_test::empty_range_parses_to_nothing()
{
    auto data = std::vector<telnetpp::u8>{};
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::parse(begin, end);
    
    CPPUNIT_ASSERT_EQUAL(size_t{0}, result.size());
    CPPUNIT_ASSERT(begin == end);
}

void parser_test::normal_character_parses_to_string()
{
    auto data = std::vector<telnetpp::u8>{ 'x' };
    std::string expected0 = "x";

    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::parse(begin, end);
    
    CPPUNIT_ASSERT_EQUAL(size_t{1}, result.size());
    CPPUNIT_ASSERT(begin == end);
    
    std::string actual = boost::get<std::string>(result[0]);
    
    CPPUNIT_ASSERT_EQUAL(expected0, actual);
}

void parser_test::two_normal_characters_parse_to_string()
{
    auto data = std::vector<telnetpp::u8>{ 'x', 'y' };
    std::string expected0 = "xy";

    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::parse(begin, end);
    
    CPPUNIT_ASSERT_EQUAL(size_t{1}, result.size());
    CPPUNIT_ASSERT(begin == end);
    
    std::string actual = boost::get<std::string>(result[0]);
    
    CPPUNIT_ASSERT_EQUAL(expected0, actual);
}

void parser_test::lone_iac_parses_to_nothing()
{
    auto data = std::vector<telnetpp::u8>{ 0xFF };
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::parse(begin, end);
    CPPUNIT_ASSERT_EQUAL(size_t{0}, result.size());
    CPPUNIT_ASSERT(begin == data.begin());
}

void parser_test::double_iac_parses_to_iac()
{
    auto data = std::vector<telnetpp::u8>{ 0xFF, 0xFF };
    auto expected0 = std::string("\xFF");
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::parse(begin, end);
    CPPUNIT_ASSERT_EQUAL(size_t{1}, result.size());
    CPPUNIT_ASSERT_EQUAL(expected0, boost::get<std::string>(result[0]));
    CPPUNIT_ASSERT(begin == end);
}

void parser_test::iac_nop_parses_to_command()
{
    auto data = std::vector<telnetpp::u8>{ 0xFF, 0xF1 };
    auto expected0 = telnetpp::command(telnetpp::nop);
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::parse(begin, end);
    CPPUNIT_ASSERT_EQUAL(size_t{1}, result.size());
    CPPUNIT_ASSERT_EQUAL(expected0, boost::get<telnetpp::command>(result[0]));
    CPPUNIT_ASSERT(begin == end);
}

void parser_test::iac_dm_parses_to_command()
{
    auto data = std::vector<telnetpp::u8>{ 0xFF, 0xF2 };
    auto expected0 = telnetpp::command(telnetpp::dm);
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::parse(begin, end);
    CPPUNIT_ASSERT_EQUAL(size_t{1}, result.size());
    CPPUNIT_ASSERT_EQUAL(expected0, boost::get<telnetpp::command>(result[0]));
    CPPUNIT_ASSERT(begin == end);
}

void parser_test::iac_brk_parses_to_command()
{
    auto data = std::vector<telnetpp::u8>{ 0xFF, 0xF3 };
    auto expected0 = telnetpp::command(telnetpp::brk);
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::parse(begin, end);
    CPPUNIT_ASSERT_EQUAL(size_t{1}, result.size());
    CPPUNIT_ASSERT_EQUAL(expected0, boost::get<telnetpp::command>(result[0]));
    CPPUNIT_ASSERT(begin == end);
}

void parser_test::iac_ip_parses_to_command()
{
    auto data = std::vector<telnetpp::u8>{ 0xFF, 0xF4 };
    auto expected0 = telnetpp::command(telnetpp::ip);
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::parse(begin, end);
    CPPUNIT_ASSERT_EQUAL(size_t{1}, result.size());
    CPPUNIT_ASSERT_EQUAL(expected0, boost::get<telnetpp::command>(result[0]));
    CPPUNIT_ASSERT(begin == end);
}

void parser_test::iac_ao_parses_to_command()
{
    auto data = std::vector<telnetpp::u8>{ 0xFF, 0xF5 };
    auto expected0 = telnetpp::command(telnetpp::ao);
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::parse(begin, end);
    CPPUNIT_ASSERT_EQUAL(size_t{1}, result.size());
    CPPUNIT_ASSERT_EQUAL(expected0, boost::get<telnetpp::command>(result[0]));
    CPPUNIT_ASSERT(begin == end);
}

void parser_test::iac_ayt_parses_to_command()
{
    auto data = std::vector<telnetpp::u8>{ 0xFF, 0xF6 };
    auto expected0 = telnetpp::command(telnetpp::ayt);
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::parse(begin, end);
    CPPUNIT_ASSERT_EQUAL(size_t{1}, result.size());
    CPPUNIT_ASSERT_EQUAL(expected0, boost::get<telnetpp::command>(result[0]));
    CPPUNIT_ASSERT(begin == end);
}

void parser_test::iac_ec_parses_to_command()
{
    auto data = std::vector<telnetpp::u8>{ 0xFF, 0xF7 };
    auto expected0 = telnetpp::command(telnetpp::ec);
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::parse(begin, end);
    CPPUNIT_ASSERT_EQUAL(size_t{1}, result.size());
    CPPUNIT_ASSERT_EQUAL(expected0, boost::get<telnetpp::command>(result[0]));
    CPPUNIT_ASSERT(begin == end);
}

void parser_test::iac_el_parses_to_command()
{
    auto data = std::vector<telnetpp::u8>{ 0xFF, 0xF8 };
    auto expected0 = telnetpp::command(telnetpp::el);
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::parse(begin, end);
    CPPUNIT_ASSERT_EQUAL(size_t{1}, result.size());
    CPPUNIT_ASSERT_EQUAL(expected0, boost::get<telnetpp::command>(result[0]));
    CPPUNIT_ASSERT(begin == end);
}

void parser_test::iac_ga_parses_to_command()
{
    auto data = std::vector<telnetpp::u8>{ 0xFF, 0xF9 };
    auto expected0 = telnetpp::command(telnetpp::ga);
    
    auto begin = data.begin();
    auto end   = data.end();
    
    auto result = telnetpp::parse(begin, end);
    CPPUNIT_ASSERT_EQUAL(size_t{1}, result.size());
    CPPUNIT_ASSERT_EQUAL(expected0, boost::get<telnetpp::command>(result[0]));
    CPPUNIT_ASSERT(begin == end);
}
