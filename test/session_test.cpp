#include "telnetpp/session.hpp"
#include "telnetpp/options/echo/client.hpp"
#include "telnetpp/options/echo/server.hpp"
#include <gtest/gtest.h>

using namespace telnetpp::literals;

namespace {

class fake_client_option : public telnetpp::client_option
{
public:
    explicit fake_client_option(telnetpp::option_type option)
      : telnetpp::client_option(option)
    {
    }

    boost::signals2::signal<
        void (telnetpp::bytes data)
    > on_subnegotiation;

private:
    void handle_subnegotiation(
        telnetpp::bytes data,
        std::function<void (telnetpp::element const &)> const &cont) override
    {
        on_subnegotiation(data);
    }
};

class fake_server_option : public telnetpp::server_option
{
public:
    explicit fake_server_option(telnetpp::option_type option)
      : telnetpp::server_option(option)
    {
    }

    boost::signals2::signal<
        void (telnetpp::bytes data)
    > on_subnegotiation;

private:
    void handle_subnegotiation(
        telnetpp::bytes data,
        std::function<void (telnetpp::element const &)> const &cont) override
    {
        on_subnegotiation(data);
    }
};

}

TEST(session_test, reception_of_text_routes_to_user_supplied_function)
{
    telnetpp::session session;
    
    static constexpr telnetpp::byte const content[] = {
        'T', 'E', 'S', 'T', ' ', 'S', 'T', 'R', 'I', 'N', 'G'
    };

    static std::string const expected_content = "TEST STRING";
    std::string received_content;
    
    static std::vector<telnetpp::byte> expected_result = {
    };

    std::vector<telnetpp::byte> received_result;
    session.receive(
        content,
        [&received_content](telnetpp::bytes content, auto const &cont)
        {
            std::transform(
                content.begin(),
                content.end(),
                std::back_inserter(received_content),
                [](auto ch) { return static_cast<char>(ch); });
        },
        [&received_result](telnetpp::bytes data)
        {
            received_result.insert(
                received_result.end(),
                data.begin(),
                data.end());
        });

    ASSERT_EQ(expected_result, received_result);
    ASSERT_EQ(expected_content, received_content);
}

TEST(session_test, reception_of_command_routes_to_installed_command_function)
{
    telnetpp::session session;

    session.install(
        telnetpp::ayt,
        [](telnetpp::command const &cmd, auto &&send)
        {
            ASSERT_EQ(telnetpp::ayt, cmd.value());

            static constexpr telnetpp::byte content[] = { 'Y', 'e', 's' };
            send(content);
        });

    session.install(
        telnetpp::dm,
        [](telnetpp::command const &cmd, auto &&send)
        {
            ASSERT_EQ(telnetpp::dm, cmd.value());

            static constexpr telnetpp::byte content[] = { 'N', 'o' };
            send(content);
        });

    static constexpr telnetpp::command const command{telnetpp::ayt};

    static constexpr telnetpp::byte const content[] = {
        telnetpp::iac, telnetpp::ayt
    };

    static std::vector<telnetpp::byte> expected_result = {
        'Y', 'e', 's'
    };

    std::vector<telnetpp::byte> received_result;
    session.receive(
        content,
        [](auto &&...){},
        [&received_result](telnetpp::bytes data)
        {
            received_result.insert(
                received_result.end(),
                data.begin(),
                data.end());
        });

    ASSERT_EQ(expected_result, received_result);
}

TEST(session_test, reception_of_negotiation_routes_to_installed_client_option)
{
    telnetpp::session session;
    telnetpp::options::echo::client client;

    session.install(client);

    telnetpp::byte const content[] = {
        telnetpp::iac, telnetpp::will, client.option_code()
    };

    std::vector<telnetpp::byte> const expected_result = {
        telnetpp::iac, telnetpp::do_, client.option_code()
    };

    std::string received_content;
    std::vector<telnetpp::byte> received_result;
    session.receive(
        content,
        [&received_content](telnetpp::bytes content, auto const &cont)
        {
            std::transform(
                content.begin(),
                content.end(),
                std::back_inserter(received_content),
                [](auto ch) { return static_cast<char>(ch); });
        },
        [&received_result](telnetpp::bytes data)
        {
            received_result.insert(
                received_result.end(),
                data.begin(),
                data.end());
        });

    ASSERT_EQ(std::string{}, received_content);
    ASSERT_EQ(expected_result, received_result);
}

TEST(session_test, reception_of_negotiation_routes_to_installed_server_option)
{
    telnetpp::session session;
    telnetpp::options::echo::server server;

    session.install(server);

    telnetpp::byte const content[] = {
        telnetpp::iac, telnetpp::do_, server.option_code()
    };

    std::vector<telnetpp::byte> const expected_result = {
        telnetpp::iac, telnetpp::will, server.option_code()
    };

    std::string received_content;
    std::vector<telnetpp::byte> received_result;
    session.receive(
        content,
        [&received_content](telnetpp::bytes content, auto const &cont)
        {
            std::transform(
                content.begin(),
                content.end(),
                std::back_inserter(received_content),
                [](auto ch) { return static_cast<char>(ch); });
        },
        [&received_result](telnetpp::bytes data)
        {
            received_result.insert(
                received_result.end(),
                data.begin(),
                data.end());
        });

    ASSERT_EQ(std::string{}, received_content);
    ASSERT_EQ(expected_result, received_result);
}

TEST(session_test, reception_of_subnegotiation_routes_to_installed_client_option)
{
    telnetpp::session session;
    fake_client_option client{0xA5};
    client.negotiate(telnetpp::will, [](auto &&...){});
    assert(client.active());

    session.install(client);

    std::vector<telnetpp::byte> const expected_subnegotiation = {
        'T', 'E', 'S', 'T'
    };
    
    std::vector<telnetpp::byte> received_subnegotiation;
    client.on_subnegotiation.connect(
        [&](telnetpp::bytes data)
        {
            received_subnegotiation.insert(
                received_subnegotiation.end(),
                data.begin(),
                data.end());
        });

    telnetpp::byte const content[] = {
        telnetpp::iac, telnetpp::sb, client.option_code(),
        'T', 'E', 'S', 'T',
        telnetpp::iac, telnetpp::se
    };

    std::vector<telnetpp::byte> const expected_result = {
    };

    
    std::string received_content;
    std::vector<telnetpp::byte> received_result;
    session.receive(
        content,
        [&received_content](telnetpp::bytes content, auto const &cont)
        {
            std::transform(
                content.begin(),
                content.end(),
                std::back_inserter(received_content),
                [](auto ch) { return static_cast<char>(ch); });
        },
        [&received_result](telnetpp::bytes data)
        {
            received_result.insert(
                received_result.end(),
                data.begin(),
                data.end());
        });

    ASSERT_EQ(std::string{}, received_content);
    ASSERT_EQ(expected_result, received_result);
    ASSERT_EQ(expected_subnegotiation, received_subnegotiation);
}

TEST(session_test, reception_of_fragmented_subnegotiation_routes_to_installed_client_option)
{
    telnetpp::session session;
    fake_client_option client{0xA5};
    client.negotiate(telnetpp::will, [](auto &&...){});
    assert(client.active());

    session.install(client);

    std::vector<telnetpp::byte> const expected_subnegotiation = {
        'T', 'E', 'S', 'T'
    };
    
    std::vector<telnetpp::byte> received_subnegotiation;
    client.on_subnegotiation.connect(
        [&](telnetpp::bytes data)
        {
            received_subnegotiation.insert(
                received_subnegotiation.end(),
                data.begin(),
                data.end());
        });

    telnetpp::byte const content0[] = {
        telnetpp::iac, telnetpp::sb, client.option_code(),
        'T', 'E'
    };
    
    telnetpp::byte const content1[] = {
        'S', 'T',
        telnetpp::iac, telnetpp::se
    };

    std::vector<telnetpp::byte> const expected_result = {
    };

    
    std::string received_content;
    auto const &receive_content = 
        [&received_content](telnetpp::bytes content, auto const &cont)
        {
            std::transform(
                content.begin(),
                content.end(),
                std::back_inserter(received_content),
                [](auto ch) { return static_cast<char>(ch); });
        };
        
    std::vector<telnetpp::byte> received_result;
    auto const &receive_result =
        [&received_result](telnetpp::bytes data)
        {
            received_result.insert(
                received_result.end(),
                data.begin(),
                data.end());
        };
        
    session.receive(content0, receive_content, receive_result);
    session.receive(content1, receive_content, receive_result);

    ASSERT_EQ(std::string{}, received_content);
    ASSERT_EQ(expected_result, received_result);
    ASSERT_EQ(expected_subnegotiation, received_subnegotiation);
}

TEST(session_test, unrouted_will_sends_dont)
{
    telnetpp::session session;

    telnetpp::byte const content[] = {
        telnetpp::iac, telnetpp::will, 0xAB
    };

    std::vector<telnetpp::byte> const expected_result = {
        telnetpp::iac, telnetpp::dont, 0xAB
    };

    std::string received_content;
    std::vector<telnetpp::byte> received_result;
    session.receive(
        content,
        [&received_content](telnetpp::bytes content, auto const &cont)
        {
            std::transform(
                content.begin(),
                content.end(),
                std::back_inserter(received_content),
                [](auto ch) { return static_cast<char>(ch); });
        },
        [&received_result](telnetpp::bytes data)
        {
            received_result.insert(
                received_result.end(),
                data.begin(),
                data.end());
        });

    ASSERT_EQ(std::string{}, received_content);
    ASSERT_EQ(expected_result, received_result);
}

TEST(session_test, unrouted_wont_results_in_dont)
{
    telnetpp::session session;

    telnetpp::byte const content[] = {
        telnetpp::iac, telnetpp::wont, 0xAB
    };

    std::vector<telnetpp::byte> const expected_result = {
        telnetpp::iac, telnetpp::dont, 0xAB
    };

    std::string received_content;
    std::vector<telnetpp::byte> received_result;
    session.receive(
        content,
        [&received_content](telnetpp::bytes content, auto const &cont)
        {
            std::transform(
                content.begin(),
                content.end(),
                std::back_inserter(received_content),
                [](auto ch) { return static_cast<char>(ch); });
        },
        [&received_result](telnetpp::bytes data)
        {
            received_result.insert(
                received_result.end(),
                data.begin(),
                data.end());
        });

    ASSERT_EQ(std::string{}, received_content);
    ASSERT_EQ(expected_result, received_result);
}

TEST(session_test, unrouted_do_results_in_wont)
{
    telnetpp::session session;

    telnetpp::byte const content[] = {
        telnetpp::iac, telnetpp::do_, 0xAB
    };

    std::vector<telnetpp::byte> const expected_result = {
        telnetpp::iac, telnetpp::wont, 0xAB
    };

    std::string received_content;
    std::vector<telnetpp::byte> received_result;
    session.receive(
        content,
        [&received_content](telnetpp::bytes content, auto const &cont)
        {
            std::transform(
                content.begin(),
                content.end(),
                std::back_inserter(received_content),
                [](auto ch) { return static_cast<char>(ch); });
        },
        [&received_result](telnetpp::bytes data)
        {
            received_result.insert(
                received_result.end(),
                data.begin(),
                data.end());
        });

    ASSERT_EQ(std::string{}, received_content);
    ASSERT_EQ(expected_result, received_result);
}

TEST(session_test, unrouted_dont_results_in_wont)
{
    telnetpp::session session;

    telnetpp::byte const content[] = {
        telnetpp::iac, telnetpp::dont, 0xAB
    };

    telnetpp::byte_storage const expected_result = {
        telnetpp::iac, telnetpp::wont, 0xAB
    };

    telnetpp::byte_storage received_content;
    telnetpp::byte_storage received_result;
    session.receive(
        content,
        [&received_content](telnetpp::bytes content, auto const &cont)
        {
            received_content.insert(
                received_content.end(),
                content.begin(),
                content.end());
        },
        [&received_result](telnetpp::bytes data)
        {
            received_result.insert(
                received_result.end(),
                data.begin(),
                data.end());
        });

    ASSERT_EQ(telnetpp::byte_storage{}, received_content);
    ASSERT_EQ(expected_result, received_result);
}

TEST(session_test, commands_can_be_sent_separately)
{
    telnetpp::session session;

    std::vector<telnetpp::byte> const expected_result = {
        telnetpp::iac, telnetpp::ayt
    };

    std::vector<telnetpp::byte> received_result;
    session.send(
        telnetpp::command(telnetpp::ayt),
        [&received_result](telnetpp::bytes data)
        {
            received_result.insert(
                received_result.end(),
                data.begin(),
                data.end());
        });

    ASSERT_EQ(expected_result, received_result);
}

TEST(session_test, non_telnet_data_can_be_sent_separately)
{
    telnetpp::session session;

    std::vector<telnetpp::byte> const expected_result = {
        'T', 'E', 'S', 'T', 0xFF, 0xFF
    };

    static constexpr telnetpp::byte const content[] = {
        'T', 'E', 'S', 'T', 0xFF
    };

    std::vector<telnetpp::byte> received_result;
    session.send(
        content,
        [&received_result](telnetpp::bytes data)
        {
            received_result.insert(
                received_result.end(),
                data.begin(),
                data.end());
        });

    ASSERT_EQ(expected_result, received_result);
}

TEST(session_test, telnet_data_can_be_received_piecemeal)
{
    telnetpp::session session;
    fake_client_option client(0xD0);
    fake_server_option server(0xD7);

    telnetpp::byte_storage received_subnegotiation_content;
    server.on_subnegotiation.connect(
        [&](telnetpp::bytes content)
        {
            received_subnegotiation_content.insert(
                received_subnegotiation_content.end(),
                content.begin(),
                content.end());
        });
    
    session.install(client);
    session.install(server);

    static auto const content = 
        "HELLO"
        "\xFF\xFB\xD0"
        "WORLD" "\xFF\xFF"
        "\xFF\xFD\xD7"
        "\xFF\xFA\xD7" "TEST" "\xFF\xF0"_tb;

    telnetpp::byte_storage received_content;
    telnetpp::byte_storage received_result;

    for (auto ch : content)
    {
        telnetpp::bytes const byte_content { &ch, 1 };
        session.receive(
            byte_content,
            [&received_content](telnetpp::bytes content, auto const &cont)
            {
                received_content.insert(
                    received_content.end(),
                    content.begin(),
                    content.end());
            },
            [&received_result](telnetpp::bytes data)
            {
                received_result.insert(
                    received_result.end(),
                    data.begin(),
                    data.end());
            });
    }

    static auto const expected_content = "HELLOWORLD" "\xFF"_tb;
    static auto const expected_result =
        "\xFF\xFD\xD0"
        "\xFF\xFB\xD7"_tb;
    static auto const expected_subnegotiation_content = "TEST"_tb;

    ASSERT_EQ(expected_content, received_content);
    ASSERT_EQ(expected_result, received_result);
    ASSERT_EQ(expected_subnegotiation_content, received_subnegotiation_content);

    ASSERT_TRUE(client.active());
    ASSERT_TRUE(server.active());
}