#include "telnetpp/options/naws/client.hpp"
#include "telnetpp/options/naws.hpp"
#include "telnetpp/protocol.hpp"
#include "expect_tokens.hpp"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class naws_client_test : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(naws_client_test);
        CPPUNIT_TEST(option_is_naws);
        CPPUNIT_TEST(valid_subnegotiation_signals_window_size_change);
        CPPUNIT_TEST(short_subnegotiation_is_ignored);
    CPPUNIT_TEST_SUITE_END();
    
private :
    void option_is_naws();
    void valid_subnegotiation_signals_window_size_change();
    void short_subnegotiation_is_ignored();
};

CPPUNIT_TEST_SUITE_REGISTRATION(naws_client_test);

void naws_client_test::option_is_naws()
{
    telnetpp::options::naws::client client;
    CPPUNIT_ASSERT_EQUAL(telnetpp::options::naws::option, client.option());
}

void naws_client_test::valid_subnegotiation_signals_window_size_change()
{
    telnetpp::options::naws::client client;
    client.activate();
    client.negotiate(telnetpp::will);
    
    telnetpp::u16 width = 0, height = 0;
    
    client.on_window_size_changed.connect(
        [&width, &height](telnetpp::u16 new_width, telnetpp::u16 new_height)
            -> std::vector<telnetpp::token_pass>
        {
            width = new_width;
            height = new_height;
            return {};
        });

    client.subnegotiate({0x01, 0x02, 0x03, 0x04});
    
    telnetpp::u16 const expected_width  = 0x01 << 8 | 0x02;
    telnetpp::u16 const expected_height = 0x03 << 8 | 0x04;
    
    CPPUNIT_ASSERT_EQUAL(expected_width, width);
    CPPUNIT_ASSERT_EQUAL(expected_height, height);
}

void naws_client_test::short_subnegotiation_is_ignored()
{
    telnetpp::options::naws::client client;
    client.activate();
    client.negotiate(telnetpp::will);
    
    bool called = false;
    client.on_window_size_changed.connect(
        [&called](auto &&, auto&&) -> std::vector<telnetpp::token_pass> 
        {
            called = true;
            return {};
        });
   
   client.subnegotiate({0x01, 0x02, 0x03});
   
   CPPUNIT_ASSERT_EQUAL(false, called);
}
