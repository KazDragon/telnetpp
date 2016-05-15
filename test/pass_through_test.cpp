#include "telnetpp/client_option.hpp"
#include "telnetpp/protocol.hpp"
#include "expect_elements.hpp"
#include <gtest/gtest.h>

namespace {

static telnetpp::u8 pass_through_option_id = 0xAB;
static std::string const test_string = "TEST";

class pass_through_option : public telnetpp::client_option
{
public :  
    pass_through_option()
      : telnetpp::client_option(pass_through_option_id)
    {
        on_state_changed.connect(
            [](telnetpp::client_option::state) -> std::vector<telnetpp::token>
            {
                return { boost::any(test_string) };
            });
    }
};

}

TEST(pass_through_test, can_pass_arbitrary_object_as_result_of_negotiation)
{
    pass_through_option option;
    option.set_activatable();
    
    expect_tokens(
        {
            telnetpp::element(telnetpp::negotiation(
                telnetpp::do_, pass_through_option_id)),
            boost::any(test_string)
        },
        option.negotiate(telnetpp::will));
}