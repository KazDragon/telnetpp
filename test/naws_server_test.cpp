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
        CPPUNIT_TEST(activation_with_no_screen_size_sends_nothing);
        CPPUNIT_TEST(setting_screen_size_when_not_activated_sends_nothing);
        CPPUNIT_TEST(activation_with_screen_size_sends_screen_size);
        CPPUNIT_TEST(setting_screen_size_when_activated_sends_screen_size);
    CPPUNIT_TEST_SUITE_END();
    
private :
    void option_is_naws();    
    void activation_with_no_screen_size_sends_nothing();
    void setting_screen_size_when_not_activated_sends_nothing();
    void activation_with_screen_size_sends_screen_size();
    void setting_screen_size_when_activated_sends_screen_size();
};

CPPUNIT_TEST_SUITE_REGISTRATION(naws_server_test);

void naws_server_test::option_is_naws()
{
    telnetpp::options::naws::server server;
    CPPUNIT_ASSERT_EQUAL(telnetpp::options::naws::option, server.option());
}

void naws_server_test::activation_with_no_screen_size_sends_nothing()
{
    telnetpp::options::naws::server server;
    server.activate();
    auto result = server.negotiate(telnetpp::do_);
    
    CPPUNIT_ASSERT_EQUAL(size_t{0}, result.size());
}

void naws_server_test::setting_screen_size_when_not_activated_sends_nothing()
{
    telnetpp::options::naws::server server;
    auto result = server.set_window_size(80, 24);
    
    CPPUNIT_ASSERT_EQUAL(size_t{0}, result.size());
}

void naws_server_test::activation_with_screen_size_sends_screen_size()
{
    telnetpp::options::naws::server server;
    server.set_window_size(80, 24);
    server.activate();
    
    auto result = server.negotiate(telnetpp::do_);
    auto expected = telnetpp::subnegotiation(
        telnetpp::options::naws::option, {0, 80, 0, 24});
    
    CPPUNIT_ASSERT_EQUAL(size_t{1}, result.size());
    CPPUNIT_ASSERT_EQUAL(
        expected, 
        boost::get<telnetpp::subnegotiation>(result[0]));
}

void naws_server_test::setting_screen_size_when_activated_sends_screen_size()
{
    telnetpp::options::naws::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
        
    auto result = server.set_window_size(80, 24);
    auto expected = telnetpp::subnegotiation(
        telnetpp::options::naws::option, {0, 80, 0, 24});
    
    CPPUNIT_ASSERT_EQUAL(size_t{1}, result.size());
    CPPUNIT_ASSERT_EQUAL(
        expected, 
        boost::get<telnetpp::subnegotiation>(result[0]));
}
