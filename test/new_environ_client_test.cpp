#include "telnetpp/options/new_environ/client.hpp"
#include "telnetpp/options/new_environ.hpp"
#include "telnetpp/protocol.hpp"
#include "expect_elements.hpp"
#include <gtest/gtest.h>
#include <utility>

TEST(new_environ_client_test, requesting_no_variables_requests_empty_list)
{
    // According to the specification, if no variables are specified in a
    // request, then it is equivalent to requesting all the variables.
    telnetpp::options::new_environ::client client;
    client.activate();
    client.negotiate(telnetpp::will);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 0x01 })
        },
        client.request_variables({}));
}

TEST(new_environ_client_test, requesting_one_var_variable_requests_user_variable)
{
    telnetpp::options::new_environ::client client;
    client.activate();
    client.negotiate(telnetpp::will);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 
                    0x01,
                    0x00, 'T', 'E', 'S', 'T'
                })
        },
        client.request_variables(
            {{
                telnetpp::options::new_environ::var,
                "TEST"
            }}));
}

TEST(new_environ_client_test, requesting_one_user_variable_requests_user_variable)
{
    telnetpp::options::new_environ::client client;
    client.activate();
    client.negotiate(telnetpp::will);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 
                    0x01,
                    0x03, 'T', 'E', 'S', 'T'
                })
        },
        client.request_variables(
            {{
                telnetpp::options::new_environ::uservar,
                "TEST"
            }}));
}

TEST(new_environ_client_test, requesting_two_variables_requests_two_variables)
{
    telnetpp::options::new_environ::client client;
    client.activate();
    client.negotiate(telnetpp::will);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 
                    0x01,
                    0x00, 'T', 'E', 'S', 'T', '0',
                    0x03, 'T', 'E', 'S', 'T', '1'
                })
        },
        client.request_variables(
            {
                {
                    telnetpp::options::new_environ::var,
                    "TEST0"
                },
                {
                    telnetpp::options::new_environ::uservar,
                    "TEST1"
                },
                
            }));
}

TEST(new_environ_client_test, requesting_uservar_with_var_in_name_is_escaped)
{
    telnetpp::options::new_environ::client client;
    client.activate();
    client.negotiate(telnetpp::will);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 
                    0x01,
                    0x03, 'T', 'E', 0x02, 0x00, 'S', 'T'
                })
        },
        client.request_variables(
            {{
                telnetpp::options::new_environ::uservar,
                std::string("TE\x00ST", 5)
            }}));
}

TEST(new_environ_client_test, requesting_uservar_with_value_in_name_is_escaped)
{
    telnetpp::options::new_environ::client client;
    client.activate();
    client.negotiate(telnetpp::will);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 
                    0x01,
                    0x03, 'T', 'E', 0x02, 0x01, 'S', 'T'
                })
        },
        client.request_variables(
            {{
                telnetpp::options::new_environ::uservar,
                std::string("TE\x01ST", 5)
            }}));
}

TEST(new_environ_client_test, requesting_uservar_with_esc_in_name_is_escaped)
{
    telnetpp::options::new_environ::client client;
    client.activate();
    client.negotiate(telnetpp::will);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 
                    0x01,
                    0x03, 'T', 'E', 0x02, 0x02, 'S', 'T'
                })
        },
        client.request_variables(
            {{
                telnetpp::options::new_environ::uservar,
                std::string("TE\x02ST", 5)
            }}));
}

TEST(new_environ_client_test, requesting_uservar_with_uservar_in_name_is_escaped)
{
    telnetpp::options::new_environ::client client;
    client.activate();
    client.negotiate(telnetpp::will);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 
                    0x01,
                    0x03, 'T', 'E', 0x02, 0x03, 'S', 'T'
                })
        },
        client.request_variables(
            {{
                telnetpp::options::new_environ::uservar,
                std::string("TE\x03ST", 5)
            }}));
}

TEST(new_environ_client_test, receiving_empty_response_does_nothing)
{
    telnetpp::options::new_environ::client client;
    client.activate();
    client.negotiate(telnetpp::will);

    bool called = false;
    client.on_variable_changed.connect(
        [&called](auto const &)
            -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
        
    client.subnegotiate({});
    
    ASSERT_FALSE(called);
}

TEST(new_environ_client_test, receiving_empty_list_does_nothing)
{
    telnetpp::options::new_environ::client client;
    client.activate();
    client.negotiate(telnetpp::will);

    bool called = false;
    client.on_variable_changed.connect(
        [&called](auto const &)
            -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
        
    client.subnegotiate({ 0x00 });
    
    ASSERT_FALSE(called);
}

TEST(new_environ_client_test, receiving_var_and_no_value_reports_undefined_var)
{
    telnetpp::options::new_environ::client client;
    client.activate();
    client.negotiate(telnetpp::will);

    telnetpp::options::new_environ::response response;
    
    client.on_variable_changed.connect(
        [&response](auto const &resp) -> std::vector<telnetpp::token>
        {
            response = resp;
            return {};
        });
        
    client.subnegotiate({
        0x00,
        0x00, 'U', 'S', 'E', 'R'
    });
    
    ASSERT_EQ(telnetpp::options::new_environ::var, response.type);
    ASSERT_EQ(std::string("USER"), response.name);
    ASSERT_FALSE(response.value.is_initialized());
}

TEST(new_environ_client_test, receiving_uservar_and_no_value_reports_undefined_uservar)
{
    telnetpp::options::new_environ::client client;
    client.activate();
    client.negotiate(telnetpp::will);

    telnetpp::options::new_environ::response response;
    
    client.on_variable_changed.connect(
        [&response](auto const &resp) -> std::vector<telnetpp::token>
        {
            response = resp;
            return {};
        });
        
    client.subnegotiate({
        0x00,
        0x03, 'T', 'E', 'S', 'T'
    });
    
    ASSERT_EQ(telnetpp::options::new_environ::uservar, response.type);
    ASSERT_EQ(std::string("TEST"), response.name);
    ASSERT_FALSE(response.value.is_initialized());
}

TEST(new_environ_client_test, receiving_var_and_empty_value_reports_empty_var)
{
    telnetpp::options::new_environ::client client;
    client.activate();
    client.negotiate(telnetpp::will);

    telnetpp::options::new_environ::response response;
    
    client.on_variable_changed.connect(
        [&response](auto const &resp) -> std::vector<telnetpp::token>
        {
            response = resp;
            return {};
        });
        
    client.subnegotiate({
        0x00,
        0x00, 'U', 'S', 'E', 'R',
        0x01,
    });
    
    ASSERT_EQ(telnetpp::options::new_environ::var, response.type);
    ASSERT_EQ(std::string("USER"), response.name);
    ASSERT_EQ(std::string(""), response.value);
}

TEST(new_environ_client_test, receiving_uservar_and_empty_value_reports_empty_var)
{
    telnetpp::options::new_environ::client client;
    client.activate();
    client.negotiate(telnetpp::will);

    telnetpp::options::new_environ::response response;
    
    client.on_variable_changed.connect(
        [&response](auto const &resp) -> std::vector<telnetpp::token>
        {
            response = resp;
            return {};
        });
        
    client.subnegotiate({
        0x00,
        0x03, 'T', 'E', 'S', 'T',
        0x01,
    });
    
    ASSERT_EQ(telnetpp::options::new_environ::uservar, response.type);
    ASSERT_EQ(std::string("TEST"), response.name);
    ASSERT_EQ(std::string(""), response.value);
}

TEST(new_environ_client_test, receiving_var_and_value_reports_var_and_value)
{
    telnetpp::options::new_environ::client client;
    client.activate();
    client.negotiate(telnetpp::will);

    telnetpp::options::new_environ::response response;
    
    client.on_variable_changed.connect(
        [&response](auto const &resp) -> std::vector<telnetpp::token>
        {
            response = resp;
            return {};
        });
        
    client.subnegotiate({
        0x00,
        0x00, 'U', 'S', 'E', 'R',
        0x01, 'B', 'O', 'B'
    });
    
    ASSERT_EQ(telnetpp::options::new_environ::var, response.type);
    ASSERT_EQ(std::string("USER"), response.name);
    ASSERT_EQ(std::string("BOB"), response.value);
}

TEST(new_environ_client_test, receiving_uservar_and_value_reports_var_and_value)
{
    telnetpp::options::new_environ::client client;
    client.activate();
    client.negotiate(telnetpp::will);

    telnetpp::options::new_environ::response response;
    
    client.on_variable_changed.connect(
        [&response](auto const &resp) -> std::vector<telnetpp::token>
        {
            response = resp;
            return {};
        });
        
    client.subnegotiate({
        0x00,
        0x03, 'T', 'E', 'S', 'T',
        0x01, 'V', 'A', 'L', 'U', 'E'
    });
    
    ASSERT_EQ(telnetpp::options::new_environ::uservar, response.type);
    ASSERT_EQ(std::string("TEST"), response.name);
    ASSERT_EQ(std::string("VALUE"), response.value);
}

TEST(new_environ_client_test, receiving_multiple_variables_reports_multiple_variables)
{
    telnetpp::options::new_environ::client client;
    client.activate();
    client.negotiate(telnetpp::will);

    std::vector<telnetpp::options::new_environ::response> responses;
    
    client.on_variable_changed.connect(
        [&responses](auto const &resp) -> std::vector<telnetpp::token>
        {
            responses.push_back(resp);
            return {};
        });
        
    client.subnegotiate({
        0x00,
        0x00, 'U', 'S', 'E', 'R',
        0x01, 'B', 'O', 'B',
        0x03, 'T', 'E', 'S', 'T', '0',
        0x03, 'T', 'E', 'S', 'T', '1',
        0x01, 'V', 'A', 'L', 'U', 'E'
    });
    
    ASSERT_EQ(std::size_t{3}, responses.size());
    
    ASSERT_EQ(telnetpp::options::new_environ::var, responses[0].type);
    ASSERT_EQ(std::string("USER"), responses[0].name);
    ASSERT_EQ(std::string("BOB"), *responses[0].value);
    
    ASSERT_EQ(telnetpp::options::new_environ::uservar, responses[1].type);
    ASSERT_EQ(std::string("TEST0"), responses[1].name);
    ASSERT_FALSE(responses[1].value.is_initialized());
    
    
    ASSERT_EQ(telnetpp::options::new_environ::uservar, responses[2].type);
    ASSERT_EQ(std::string("TEST1"), responses[2].name);
    ASSERT_EQ(std::string("VALUE"), *responses[2].value);
}

TEST(new_environ_client_test, receiving_info_for_multiple_variables_reports_multiple_variables)
{
    telnetpp::options::new_environ::client client;
    client.activate();
    client.negotiate(telnetpp::will);

    std::vector<telnetpp::options::new_environ::response> responses;
    
    client.on_variable_changed.connect(
        [&responses](auto const &resp) -> std::vector<telnetpp::token>
        {
            responses.push_back(resp);
            return {};
        });
        
    client.subnegotiate({
        0x02,
        0x00, 'U', 'S', 'E', 'R',
        0x01, 'B', 'O', 'B',
        0x03, 'T', 'E', 'S', 'T', '0',
        0x03, 'T', 'E', 'S', 'T', '1',
        0x01, 'V', 'A', 'L', 'U', 'E'
    });
    
    ASSERT_EQ(std::size_t{3}, responses.size());
    
    ASSERT_EQ(telnetpp::options::new_environ::var, responses[0].type);
    ASSERT_EQ(std::string("USER"), responses[0].name);
    ASSERT_EQ(std::string("BOB"), *responses[0].value);
    
    ASSERT_EQ(telnetpp::options::new_environ::uservar, responses[1].type);
    ASSERT_EQ(std::string("TEST0"), responses[1].name);
    ASSERT_FALSE(responses[1].value.is_initialized());
    
    
    ASSERT_EQ(telnetpp::options::new_environ::uservar, responses[2].type);
    ASSERT_EQ(std::string("TEST1"), responses[2].name);
    ASSERT_EQ(std::string("VALUE"), *responses[2].value);
}

TEST(new_environ_client_test, receiving_uservar_with_escaped_var_in_name_reports_variable_correctly)
{
    telnetpp::options::new_environ::client client;
    client.activate();
    client.negotiate(telnetpp::will);

    telnetpp::options::new_environ::response response;
    
    client.on_variable_changed.connect(
        [&response](auto const &resp) -> std::vector<telnetpp::token>
        {
            response = resp;
            return {};
        });

    client.subnegotiate({
        0x00,
        0x03, 'T', 'E', 0x02, 0x00, 'S', 'T',
        0x01, 'V', 'A', 'L', 'U', 'E'
    });
    
    ASSERT_EQ(telnetpp::options::new_environ::uservar, response.type);
    ASSERT_EQ(std::string("TE\x00ST", 5), response.name);
    ASSERT_EQ(std::string("VALUE"), response.value);
}

TEST(new_environ_client_test, receiving_uservar_with_escaped_uservar_in_name_reports_variable_correctly)
{
    telnetpp::options::new_environ::client client;
    client.activate();
    client.negotiate(telnetpp::will);

    telnetpp::options::new_environ::response response;
    
    client.on_variable_changed.connect(
        [&response](auto const &resp) -> std::vector<telnetpp::token>
        {
            response = resp;
            return {};
        });

    client.subnegotiate({
        0x00,
        0x03, 'T', 'E', 0x02, 0x03, 'S', 'T',
        0x01, 'V', 'A', 'L', 'U', 'E'
    });
    
    ASSERT_EQ(telnetpp::options::new_environ::uservar, response.type);
    ASSERT_EQ(std::string("TE\x03ST", 5), response.name);
    ASSERT_EQ(std::string("VALUE"), response.value);
}

TEST(new_environ_client_test, receiving_uservar_with_escaped_value_in_name_reports_variable_correctly)
{
    telnetpp::options::new_environ::client client;
    client.activate();
    client.negotiate(telnetpp::will);

    telnetpp::options::new_environ::response response;
    
    client.on_variable_changed.connect(
        [&response](auto const &resp) -> std::vector<telnetpp::token>
        {
            response = resp;
            return {};
        });

    client.subnegotiate({
        0x00,
        0x03, 'T', 'E', 0x02, 0x01, 'S', 'T',
        0x01, 'V', 'A', 'L', 'U', 'E'
    });
    
    ASSERT_EQ(telnetpp::options::new_environ::uservar, response.type);
    ASSERT_EQ(std::string("TE\x01ST", 5), response.name);
    ASSERT_EQ(std::string("VALUE"), response.value);
}

TEST(new_environ_client_test, receiving_uservar_with_escaped_var_in_value_reports_variable_correctly)
{
    telnetpp::options::new_environ::client client;
    client.activate();
    client.negotiate(telnetpp::will);

    telnetpp::options::new_environ::response response;
    
    client.on_variable_changed.connect(
        [&response](auto const &resp) -> std::vector<telnetpp::token>
        {
            response = resp;
            return {};
        });

    client.subnegotiate({
        0x00,
        0x03, 'T', 'E', 'S', 'T',
        0x01, 'V', 'A', 0x02, 0x00, 'L', 'U', 'E'
    });
    
    ASSERT_EQ(telnetpp::options::new_environ::uservar, response.type);
    ASSERT_EQ(std::string("TEST"), response.name);
    ASSERT_EQ(std::string("VA\x00LUE", 6), response.value);
}

TEST(new_environ_client_test, receiving_uservar_with_escaped_uservar_in_value_reports_variable_correctly)
{
    telnetpp::options::new_environ::client client;
    client.activate();
    client.negotiate(telnetpp::will);

    telnetpp::options::new_environ::response response;
    
    client.on_variable_changed.connect(
        [&response](auto const &resp) -> std::vector<telnetpp::token>
        {
            response = resp;
            return {};
        });

    client.subnegotiate({
        0x00,
        0x03, 'T', 'E', 'S', 'T',
        0x01, 'V', 'A', 0x02, 0x03, 'L', 'U', 'E'
    });
    
    ASSERT_EQ(telnetpp::options::new_environ::uservar, response.type);
    ASSERT_EQ(std::string("TEST"), response.name);
    ASSERT_EQ(std::string("VA\x03LUE", 6), response.value);
}
