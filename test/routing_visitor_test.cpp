#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class routing_visitor_test : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(routing_visitor_test);
        CPPUNIT_TEST(dummy_test);
    CPPUNIT_TEST_SUITE_END();

private :
    void dummy_test();
};


CPPUNIT_TEST_SUITE_REGISTRATION(routing_visitor_test);

void routing_visitor_test::dummy_test()
{
}