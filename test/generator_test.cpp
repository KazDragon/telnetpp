#include "telnetpp/generator.hpp"
#include "telnetpp/protocol.hpp"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class generator_test : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(generator_test);
        CPPUNIT_TEST(empty_array_generates_nothing);
        CPPUNIT_TEST(empty_string_generates_nothing);
        CPPUNIT_TEST(string_with_content_generates_string);
        CPPUNIT_TEST(string_with_iac_content_generates_doubled_iac);
        CPPUNIT_TEST(command_generates_command);
        CPPUNIT_TEST(negotiation_generates_negotiation);
        CPPUNIT_TEST(empty_subnegotiation_generates_empty_subnegotiation);
        CPPUNIT_TEST(subnegotiation_with_content_generates_subnegotiation_with_content);
        CPPUNIT_TEST(subnegotiation_with_iac_content_generates_doubled_iac);
        CPPUNIT_TEST(many_elements_generates_many_elements);
    CPPUNIT_TEST_SUITE_END();
    
private  :
    void empty_array_generates_nothing();
    void empty_string_generates_nothing();
    void string_with_content_generates_string();
    void string_with_iac_content_generates_doubled_iac();
    void command_generates_command();
    void negotiation_generates_negotiation();
    void empty_subnegotiation_generates_empty_subnegotiation();
    void subnegotiation_with_content_generates_subnegotiation_with_content();
    void subnegotiation_with_iac_content_generates_doubled_iac();
    void many_elements_generates_many_elements();
};

CPPUNIT_TEST_SUITE_REGISTRATION(generator_test);

template <class InputIterator>
void expect(
    InputIterator begin, 
    InputIterator end,
    std::vector<telnetpp::u8> const &expectation)
{
    auto result = telnetpp::generate(begin, end);
    
    if (expectation.empty())
    {
        CPPUNIT_ASSERT_EQUAL(size_t{0}, result.size());
    }
    else
    {
        auto stream = boost::get<std::vector<telnetpp::u8>>(
            result[0]);
            
        CPPUNIT_ASSERT_EQUAL(expectation.size(), stream.size());
        
        for (size_t index = 0; index < expectation.size(); ++index)
        {
            CPPUNIT_ASSERT_EQUAL(expectation[index], stream[index]);
        }
    }
}

template <class Collection>
void expect(
    Collection &&collection,
    std::vector<telnetpp::u8> const &expectation)
{
    auto result = telnetpp::generate(std::forward<Collection>(collection));
    
    if (expectation.empty())
    {
        CPPUNIT_ASSERT_EQUAL(size_t{0}, result.size());
    }
    else
    {
        auto stream = boost::get<std::vector<telnetpp::u8>>(
            result[0]);

        CPPUNIT_ASSERT_EQUAL(expectation.size(), stream.size());
        
        for (size_t index = 0; index < expectation.size(); ++index)
        {
            CPPUNIT_ASSERT_EQUAL(expectation[index], stream[index]);
        }
    }
}

void generator_test::empty_array_generates_nothing()
{
    std::vector<telnetpp::token> data = {};
    std::vector<telnetpp::u8> expected = {};
    
    expect(data.begin(), data.end(), expected);
}

void generator_test::empty_string_generates_nothing()
{
    std::vector<telnetpp::token> data = { 
        telnetpp::element(std::string(""))
    };
    
    std::vector<telnetpp::u8> expected = {};
    
    expect(data.begin(), data.end(), expected);
}

void generator_test::string_with_content_generates_string()
{
    std::vector<telnetpp::token> data = { 
        telnetpp::element(std::string("abcde"))
    };
    
    std::vector<telnetpp::u8> expected = { 'a', 'b', 'c', 'd', 'e' };
    
    expect(data.begin(), data.end(), expected);
}

void generator_test::string_with_iac_content_generates_doubled_iac()
{
    std::vector<telnetpp::token> data = { 
        telnetpp::element(std::string("ab\xFF""cd"))
    };
    
    std::vector<telnetpp::u8> expected = { 'a', 'b', 0xFF, 0xFF, 'c', 'd' };
    
    expect(data.begin(), data.end(), expected);
}

void generator_test::command_generates_command()
{
    std::vector<telnetpp::token> data = { 
        telnetpp::element(telnetpp::command(telnetpp::nop))
    };
    
    std::vector<telnetpp::u8> expected = { 0xFF, 0xF1 };
    
    expect(data.begin(), data.end(), expected);
}

void generator_test::negotiation_generates_negotiation()
{
    std::vector<telnetpp::token> data = { 
        telnetpp::element(telnetpp::negotiation(telnetpp::will, 0xDE))
    };
    
    std::vector<telnetpp::u8> expected = { 0xFF, 0xFB, 0xDE };
    
    expect(data.begin(), data.end(), expected);
}

void generator_test::empty_subnegotiation_generates_empty_subnegotiation()
{
    std::vector<telnetpp::token> data = {
        telnetpp::element(telnetpp::subnegotiation(0xAB, {}))
    };
    
    std::vector<telnetpp::u8> expected = { 0xFF, 0xFA, 0xAB, 0xFF, 0xF0 };
    
    expect(data.begin(), data.end(), expected);
}

void generator_test::subnegotiation_with_content_generates_subnegotiation_with_content()
{
    std::vector<telnetpp::token> data = {
        telnetpp::element(telnetpp::subnegotiation(0xCD, { 'a', 'b', 'c', 'd', 'e' }))
    };
    
    std::vector<telnetpp::u8> expected = {
        0xFF, 0xFA,
        0xCD,
        'a', 'b', 'c', 'd', 'e',
        0xFF, 0xF0
    };

    expect(data.begin(), data.end(), expected);
}

void generator_test::subnegotiation_with_iac_content_generates_doubled_iac()
{
    std::vector<telnetpp::token> data = {
        telnetpp::element(telnetpp::subnegotiation(0x74, { 'a', 'b', 0xFF, 'c', 'd' }))
    };
    
    std::vector<telnetpp::u8> expected = {
        0xFF, 0xFA,
        0x74,
        'a', 'b', 0xFF, 0xFF, 'c', 'd',
        0xFF, 0xF0
    };

    expect(data.begin(), data.end(), expected);
}

void generator_test::many_elements_generates_many_elements()
{
    std::vector<telnetpp::token> data = {
        telnetpp::element(std::string("abcd")),
        telnetpp::element(std::string("ef\xFFg")),
        telnetpp::element(telnetpp::negotiation(telnetpp::wont, 0xFF)),
        telnetpp::element(telnetpp::subnegotiation(0xFF, { 0xFF }))
    };
    
    std::vector<telnetpp::u8> expected = {
        'a', 'b', 'c', 'd', 
        'e', 'f', 0xFF, 0xFF, 'g',
        0xFF, 0xFC, 0xFF,
        0xFF, 0xFA, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0
    };
    
    expect(data, expected);
}
