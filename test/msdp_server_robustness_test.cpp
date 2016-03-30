#include "telnetpp/options/msdp/server.hpp"
#include "telnetpp/protocol.hpp"
#include "expect_elements.hpp"
#include <gtest/gtest.h>

// From a test=documented requirements point of view, this test suite documents
// how the MSDP parser handles out-of-spec data.

namespace {

static void activate_msdp_server(telnetpp::options::msdp::server &server)
{
    server.activate();
    server.negotiate(telnetpp::do_);
}

static void register_msdp_server_variable_reception(
    telnetpp::options::msdp::server &server,
    std::vector<telnetpp::options::msdp::variable> &variables)
{
    server.on_receive.connect(
        [&variables](auto const &vars) -> std::vector<telnetpp::token>
        {
            variables = vars;
            return {};
        });
}

}

TEST(msdp_server_robustness_test, early_data_is_ignored)
{
    // From a spec perspective, the only valid token that can be used to begin
    // the subnegotiation is the MSDP_VAR variable name introducer.

    // For the purpoes of this implementation, we consider everything ahead of
    // that to be unparsed "comment" that has no effect on the data received.
    telnetpp::options::msdp::server server;
    std::vector<telnetpp::options::msdp::variable> variables;

    register_msdp_server_variable_reception(server, variables);
    activate_msdp_server(server);

    server.subnegotiate({
        2, 3, 4, 5, 6, 7, 8,
        'u', 'n', 'p', 'a', 'r', 's', 'e', 'd',

        // Real subnegotiation data starts here.
        1, 'v', 'a', 'r',
        2, 'v', 'a', 'l'
    });

    telnetpp::options::msdp::variable expected{"var", "val"};

    ASSERT_EQ(size_t{1}, variables.size());
    ASSERT_EQ(expected, variables[0]);
}

TEST(msdp_server_robustness_test, extra_table_close_is_ignored)
{
    // White box test: since the parser is implemented in terms of a stack,
    // a hostile packet might attempt to crash it by sending extra table
    // closes, which would cause the stack to underflow as it is popped too
    // much.

    // The decision here is to treat the table close for the root object as
    // implicit and ignore any other attempt to close that object.

    telnetpp::options::msdp::server server;
    std::vector<telnetpp::options::msdp::variable> variables;

    register_msdp_server_variable_reception(server, variables);
    activate_msdp_server(server);

    server.subnegotiate({
        1, 'v', 'a', 'r', '0',
        2,
            4, // Note: extra table close.
        'u', 'n', 'p', 'a', 'r', 's', 'e', 'd',
        1, 'v', 'a', 'r', '1',
        2, 'v', 'a', 'l'
    });

    telnetpp::options::msdp::variable expected0{"var0", ""};
    telnetpp::options::msdp::variable expected1{"var1", "val"};

    ASSERT_EQ(size_t{2}, variables.size());
    ASSERT_EQ(expected0, variables[0]);
    ASSERT_EQ(expected1, variables[1]);
}