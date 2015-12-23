#include "telnetpp/options/new_environ/server.hpp"
#include "telnetpp/options/new_environ.hpp"
#include "telnetpp/protocol.hpp"
#include "expect_elements.hpp"
#include <gtest/gtest.h>
#include <utility>

TEST(new_environ_server_test, requesting_no_variables_requests_empty_list)
{
    // According to the specification, if no variables are specified in a
    // request, then it is equivalent to requesting all the variables.
    telnetpp::options::new_environ::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 0x01 })
        },
        server.request_variables({}));
}

TEST(new_environ_server_test, requesting_one_var_variable_requests_user_variable)
{
    telnetpp::options::new_environ::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 
                    0x01,
                    0x00, 'T', 'E', 'S', 'T'
                })
        },
        server.request_variables(
            {{
                telnetpp::options::new_environ::var,
                "TEST"
            }}));
}

TEST(new_environ_server_test, requesting_one_user_variable_requests_user_variable)
{
    telnetpp::options::new_environ::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 
                    0x01,
                    0x03, 'T', 'E', 'S', 'T'
                })
        },
        server.request_variables(
            {{
                telnetpp::options::new_environ::uservar,
                "TEST"
            }}));
}

TEST(new_environ_server_test, requesting_two_variables_requests_two_variables)
{
    telnetpp::options::new_environ::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    
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
        server.request_variables(
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

TEST(new_environ_server_test, requesting_uservar_with_var_in_name_is_escaped)
{
    telnetpp::options::new_environ::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 
                    0x01,
                    0x03, 'T', 'E', 0x02, 0x00, 'S', 'T'
                })
        },
        server.request_variables(
            {{
                telnetpp::options::new_environ::uservar,
                std::string("TE\x00ST", 5)
            }}));
}

TEST(new_environ_server_test, requesting_uservar_with_value_in_name_is_escaped)
{
    telnetpp::options::new_environ::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 
                    0x01,
                    0x03, 'T', 'E', 0x02, 0x01, 'S', 'T'
                })
        },
        server.request_variables(
            {{
                telnetpp::options::new_environ::uservar,
                std::string("TE\x01ST", 5)
            }}));
}

TEST(new_environ_server_test, requesting_uservar_with_esc_in_name_is_escaped)
{
    telnetpp::options::new_environ::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 
                    0x01,
                    0x03, 'T', 'E', 0x02, 0x02, 'S', 'T'
                })
        },
        server.request_variables(
            {{
                telnetpp::options::new_environ::uservar,
                std::string("TE\x02ST", 5)
            }}));
}

TEST(new_environ_server_test, requesting_uservar_with_uservar_in_name_is_escaped)
{
    telnetpp::options::new_environ::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 
                    0x01,
                    0x03, 'T', 'E', 0x02, 0x03, 'S', 'T'
                })
        },
        server.request_variables(
            {{
                telnetpp::options::new_environ::uservar,
                std::string("TE\x03ST", 5)
            }}));
}

TEST(new_environ_server_test, receiving_empty_response_does_nothing)
{
    telnetpp::options::new_environ::server server;
    server.activate();
    server.negotiate(telnetpp::do_);

    bool called = false;
    server.on_variable_changed.connect(
        [&called](auto const &, auto const &, auto const &)
            -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
        
    server.subnegotiate({});
    
    ASSERT_FALSE(called);
}

TEST(new_environ_server_test, receiving_empty_list_does_nothing)
{
    telnetpp::options::new_environ::server server;
    server.activate();
    server.negotiate(telnetpp::do_);

    bool called = false;
    server.on_variable_changed.connect(
        [&called](auto const &, auto const &, auto const &)
            -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
        
    server.subnegotiate({ 0x00 });
    
    ASSERT_FALSE(called);
}

TEST(new_environ_server_test, receiving_var_and_no_value_reports_undefined_var)
{
    telnetpp::options::new_environ::server server;
    server.activate();
    server.negotiate(telnetpp::do_);

    telnetpp::u8                 type;
    std::string                  name;
    boost::optional<std::string> value;
    
    server.on_variable_changed.connect(
        [&type, &name, &value](
            auto const &var_type, 
            auto const &var_name, 
            auto const &var_value)
            -> std::vector<telnetpp::token>
        {
            type = var_type;
            name = var_name;
            value = var_value;
            return {};
        });
        
    server.subnegotiate({
        0x00,
        0x00, 'U', 'S', 'E', 'R'
    });
    
    ASSERT_EQ(telnetpp::options::new_environ::var, type);
    ASSERT_EQ(std::string("USER"), name);
    ASSERT_FALSE(value.is_initialized());
}

TEST(new_environ_server_test, receiving_uservar_and_no_value_reports_undefined_uservar)
{
    telnetpp::options::new_environ::server server;
    server.activate();
    server.negotiate(telnetpp::do_);

    telnetpp::u8                 type;
    std::string                  name;
    boost::optional<std::string> value;
    
    server.on_variable_changed.connect(
        [&type, &name, &value](
            auto const &var_type, 
            auto const &var_name, 
            auto const &var_value)
            -> std::vector<telnetpp::token>
        {
            type = var_type;
            name = var_name;
            value = var_value;
            return {};
        });
        
    server.subnegotiate({
        0x00,
        0x03, 'T', 'E', 'S', 'T'
    });
    
    ASSERT_EQ(telnetpp::options::new_environ::uservar, type);
    ASSERT_EQ(std::string("TEST"), name);
    ASSERT_FALSE(value.is_initialized());
}

TEST(new_environ_server_test, receiving_var_and_empty_value_reports_empty_var)
{
    telnetpp::options::new_environ::server server;
    server.activate();
    server.negotiate(telnetpp::do_);

    telnetpp::u8                 type;
    std::string                  name;
    boost::optional<std::string> value;
    
    server.on_variable_changed.connect(
        [&type, &name, &value](
            auto const &var_type, 
            auto const &var_name, 
            auto const &var_value)
            -> std::vector<telnetpp::token>
        {
            type = var_type;
            name = var_name;
            value = var_value;
            return {};
        });
        
    server.subnegotiate({
        0x00,
        0x00, 'U', 'S', 'E', 'R',
        0x01,
    });
    
    ASSERT_EQ(telnetpp::options::new_environ::var, type);
    ASSERT_EQ(std::string("USER"), name);
    ASSERT_EQ(std::string(""), value);
}

TEST(new_environ_server_test, receiving_uservar_and_empty_value_reports_empty_var)
{
    telnetpp::options::new_environ::server server;
    server.activate();
    server.negotiate(telnetpp::do_);

    telnetpp::u8                 type;
    std::string                  name;
    boost::optional<std::string> value;
    
    server.on_variable_changed.connect(
        [&type, &name, &value](
            auto const &var_type, 
            auto const &var_name, 
            auto const &var_value)
            -> std::vector<telnetpp::token>
        {
            type = var_type;
            name = var_name;
            value = var_value;
            return {};
        });
        
    server.subnegotiate({
        0x00,
        0x03, 'T', 'E', 'S', 'T',
        0x01,
    });
    
    ASSERT_EQ(telnetpp::options::new_environ::uservar, type);
    ASSERT_EQ(std::string("TEST"), name);
    ASSERT_EQ(std::string(""), value);
}

TEST(new_environ_server_test, receiving_var_and_value_reports_var_and_value)
{
    telnetpp::options::new_environ::server server;
    server.activate();
    server.negotiate(telnetpp::do_);

    telnetpp::u8                 type;
    std::string                  name;
    boost::optional<std::string> value;
    
    server.on_variable_changed.connect(
        [&type, &name, &value](
            auto const &var_type, 
            auto const &var_name, 
            auto const &var_value)
            -> std::vector<telnetpp::token>
        {
            type = var_type;
            name = var_name;
            value = var_value;
            return {};
        });
        
    server.subnegotiate({
        0x00,
        0x00, 'U', 'S', 'E', 'R',
        0x01, 'B', 'O', 'B'
    });
    
    ASSERT_EQ(telnetpp::options::new_environ::var, type);
    ASSERT_EQ(std::string("USER"), name);
    ASSERT_EQ(std::string("BOB"), value);
}

TEST(new_environ_server_test, receiving_uservar_and_value_reports_var_and_value)
{
    telnetpp::options::new_environ::server server;
    server.activate();
    server.negotiate(telnetpp::do_);

    telnetpp::u8                 type;
    std::string                  name;
    boost::optional<std::string> value;
    
    server.on_variable_changed.connect(
        [&type, &name, &value](
            auto const &var_type, 
            auto const &var_name, 
            auto const &var_value)
            -> std::vector<telnetpp::token>
        {
            type = var_type;
            name = var_name;
            value = var_value;
            return {};
        });
        
    server.subnegotiate({
        0x00,
        0x03, 'T', 'E', 'S', 'T',
        0x01, 'V', 'A', 'L', 'U', 'E'
    });
    
    ASSERT_EQ(telnetpp::options::new_environ::uservar, type);
    ASSERT_EQ(std::string("TEST"), name);
    ASSERT_EQ(std::string("VALUE"), value);
}

TEST(new_environ_server_test, receiving_multiple_variables_reports_multiple_variables)
{
    telnetpp::options::new_environ::server server;
    server.activate();
    server.negotiate(telnetpp::do_);

    std::vector<std::tuple<
        telnetpp::u8,
        std::string,
        boost::optional<std::string>
    >> variables;

    server.on_variable_changed.connect(
        [&variables](
            auto const &var_type, 
            auto const &var_name, 
            auto const &var_value)
            -> std::vector<telnetpp::token>
        {
            variables.push_back(std::tie(var_type, var_name, var_value));
            return {};
        });
        
    server.subnegotiate({
        0x00,
        0x00, 'U', 'S', 'E', 'R',
        0x01, 'B', 'O', 'B',
        0x03, 'T', 'E', 'S', 'T', '0',
        0x03, 'T', 'E', 'S', 'T', '1',
        0x01, 'V', 'A', 'L', 'U', 'E'
    });
    
    ASSERT_EQ(std::size_t{3}, variables.size());
    
    ASSERT_EQ(telnetpp::options::new_environ::var, std::get<0>(variables[0]));
    ASSERT_EQ(std::string("USER"), std::get<1>(variables[0]));
    ASSERT_EQ(std::string("BOB"), *std::get<2>(variables[0]));
    
    ASSERT_EQ(telnetpp::options::new_environ::uservar, std::get<0>(variables[1]));
    ASSERT_EQ(std::string("TEST0"), std::get<1>(variables[1]));
    ASSERT_FALSE((std::get<2>(variables[1])).is_initialized());
    
    
    ASSERT_EQ(telnetpp::options::new_environ::uservar, std::get<0>(variables[2]));
    ASSERT_EQ(std::string("TEST1"), std::get<1>(variables[2]));
    ASSERT_EQ(std::string("VALUE"), *std::get<2>(variables[2]));
}

TEST(new_environ_server_test, receiving_info_for_multiple_variables_reports_multiple_variables)
{
    telnetpp::options::new_environ::server server;
    server.activate();
    server.negotiate(telnetpp::do_);

    std::vector<std::tuple<
        telnetpp::u8,
        std::string,
        boost::optional<std::string>
    >> variables;

    server.on_variable_changed.connect(
        [&variables](
            auto const &var_type, 
            auto const &var_name, 
            auto const &var_value)
            -> std::vector<telnetpp::token>
        {
            variables.push_back(std::tie(var_type, var_name, var_value));
            return {};
        });
        
    server.subnegotiate({
        0x02,
        0x00, 'U', 'S', 'E', 'R',
        0x01, 'B', 'O', 'B',
        0x03, 'T', 'E', 'S', 'T', '0',
        0x03, 'T', 'E', 'S', 'T', '1',
        0x01, 'V', 'A', 'L', 'U', 'E'
    });
    
    ASSERT_EQ(std::size_t{3}, variables.size());
    
    ASSERT_EQ(telnetpp::options::new_environ::var, std::get<0>(variables[0]));
    ASSERT_EQ(std::string("USER"), std::get<1>(variables[0]));
    ASSERT_EQ(std::string("BOB"), *std::get<2>(variables[0]));
    
    ASSERT_EQ(telnetpp::options::new_environ::uservar, std::get<0>(variables[1]));
    ASSERT_EQ(std::string("TEST0"), std::get<1>(variables[1]));
    ASSERT_FALSE((std::get<2>(variables[1])).is_initialized());
    
    
    ASSERT_EQ(telnetpp::options::new_environ::uservar, std::get<0>(variables[2]));
    ASSERT_EQ(std::string("TEST1"), std::get<1>(variables[2]));
    ASSERT_EQ(std::string("VALUE"), *std::get<2>(variables[2]));
}