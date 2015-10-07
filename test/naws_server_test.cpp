#include "telnetpp/options/naws/server.hpp"
#include "telnetpp/options/naws.hpp"
#include "telnetpp/protocol.hpp"
#include "expect_tokens.hpp"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class naws_server_test : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(naws_server_test);
        CPPUNIT_TEST(option_is_naws);
        CPPUNIT_TEST(valid_subnegotiation_signals_window_size_change);
    CPPUNIT_TEST_SUITE_END();
    
private :
    void option_is_naws();
    void valid_subnegotiation_signals_window_size_change();
};

CPPUNIT_TEST_SUITE_REGISTRATION(naws_server_test);

void naws_server_test::option_is_naws()
{
    telnetpp::options::naws::server server;
    CPPUNIT_ASSERT_EQUAL(telnetpp::options::naws::option, server.option());
}

void naws_server_test::valid_subnegotiation_signals_window_size_change()
{
    telnetpp::options::naws::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    
    telnetpp::u16 width = 0, height = 0;
    
    server.on_window_size_changed.connect(
        [&width, &height](telnetpp::u16 new_width, telnetpp::u16 new_height)
            -> std::vector<telnetpp::token>
        {
            width = new_width;
            height = new_height;
            return {};
        });

    server.subnegotiate({0x01, 0x02, 0x03, 0x04});
    
    telnetpp::u16 const expected_width  = 0x01 << 8 | 0x02;
    telnetpp::u16 const expected_height = 0x03 << 8 | 0x04;
    
    CPPUNIT_ASSERT_EQUAL(expected_width, width);
    CPPUNIT_ASSERT_EQUAL(expected_height, height);
}
