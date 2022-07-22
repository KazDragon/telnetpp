#include "telnetpp/session.hpp"
#include "telnetpp/client_option.hpp"
#include "telnetpp/server_option.hpp"
#include "fakes/fake_channel.hpp"
#include "fakes/fake_client_option.hpp"
#include "fakes/fake_server_option.hpp"
#include <gtest/gtest.h>

using namespace telnetpp::literals;

namespace {
class a_session : public testing::Test
{
protected:
    void async_read()
    {
        session_.async_read(
            [this](telnetpp::bytes content) {
                received_content_.append(content.begin(), content.end());

                // The session should always send empty content as the last 
                // callback to indicate completion.
                complete_ = content.empty();
            });
    }

    fake_channel channel_;
    telnetpp::session session_{channel_};

    telnetpp::byte_storage received_content_;
    bool complete_{false};
};

}

TEST_F(a_session, routed_text_to_user_supplied_function)
{
    static telnetpp::byte_storage const content = {
        'T', 'E', 'S', 'T', ' ', 'S', 'T', 'R', 'I', 'N', 'G'
    };

    async_read();
    channel_.receive(content);

    ASSERT_EQ(content, received_content_);
    ASSERT_TRUE(complete_);
}

TEST_F(a_session, routes_commands_to_installed_command_function)
{
    session_.install(
        telnetpp::ayt,
        [this](telnetpp::command const &cmd)
        {
            ASSERT_EQ(telnetpp::ayt, cmd.value());

            static constexpr telnetpp::byte content[] = { 'Y', 'e', 's' };
            session_.write(content);
        });

    session_.install(
        telnetpp::dm,
        [this](telnetpp::command const &cmd)
        {
            ASSERT_EQ(telnetpp::dm, cmd.value());

            static constexpr telnetpp::byte content[] = { 'N', 'o' };
            session_.write(content);
        });

    static constexpr telnetpp::byte const content[] = {
        telnetpp::iac, telnetpp::ayt
    };

    async_read();
    channel_.receive(content);

    static telnetpp::byte_storage const expected_result = {
        'Y', 'e', 's'
    };

    ASSERT_EQ(expected_result, channel_.written_);
    ASSERT_TRUE(received_content_.empty());
    ASSERT_TRUE(complete_);
}

TEST_F(a_session, routes_negotiations_to_installed_client_option)
{
    constexpr telnetpp::option_type option = 42;
    fake_client_option client{session_, option};
    session_.install(client);

    telnetpp::byte const content[] = {
        telnetpp::iac, telnetpp::will, client.option_code()
    };

    async_read();
    channel_.receive(content);

    telnetpp::byte_storage const expected_result = {
        telnetpp::iac, telnetpp::do_, client.option_code()
    };

    ASSERT_EQ(expected_result, channel_.written_);
    ASSERT_TRUE(complete_);
}

TEST_F(a_session, routes_negotiations_to_installed_server_option)
{
    constexpr telnetpp::option_type option = 67;
    fake_server_option server{session_, option};

    session_.install(server);

    telnetpp::byte const content[] = {
        telnetpp::iac, telnetpp::do_, server.option_code()
    };

    async_read();
    channel_.receive(content);

    telnetpp::byte_storage const expected_result = {
        telnetpp::iac, telnetpp::will, server.option_code()
    };

    ASSERT_EQ(expected_result, channel_.written_);
    ASSERT_TRUE(complete_);
}

TEST_F(a_session, routes_subnegotiations_to_installed_client_option)
{
    constexpr telnetpp::option_type option = 0xA5;
    fake_client_option client{session_, option};

    client.negotiate(telnetpp::will);
    assert(client.active());

    session_.install(client);

    telnetpp::byte_storage received_subnegotiation;
    client.on_subnegotiation.connect(
        [&](telnetpp::bytes data)
        {
            received_subnegotiation.append(
                data.begin(),
                data.end());
        });

    telnetpp::byte const content[] = {
        telnetpp::iac, telnetpp::sb, client.option_code(),
        'T', 'E', 'S', 'T',
        telnetpp::iac, telnetpp::se
    };

    async_read();
    channel_.receive(content);

    telnetpp::byte_storage const expected_subnegotiation = {
        'T', 'E', 'S', 'T'
    };
    
    ASSERT_EQ(expected_subnegotiation, received_subnegotiation);
    ASSERT_TRUE(complete_);
}

TEST_F(a_session, routes_fragmented_subnegotiations_to_installed_client_option)
{
    constexpr telnetpp::option_type option = 0xA5;
    fake_client_option client{session_, option};

    client.negotiate(telnetpp::will);
    assert(client.active());

    session_.install(client);

    telnetpp::byte_storage received_subnegotiation;
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

    async_read();
    channel_.receive(content0);

    ASSERT_TRUE(received_subnegotiation.empty());
    ASSERT_TRUE(std::exchange(complete_, false));

    async_read();
    channel_.receive(content1);
    
    telnetpp::byte_storage const expected_subnegotiation = {
        'T', 'E', 'S', 'T'
    };

    ASSERT_EQ(expected_subnegotiation, received_subnegotiation);
    ASSERT_TRUE(received_content_.empty());
    ASSERT_TRUE(complete_);
}

TEST_F(a_session, sends_dont_for_unrouted_will)
{
    static telnetpp::byte_storage const content = {
        telnetpp::iac, telnetpp::will, 0xAB
    };

    async_read();
    channel_.receive(content);

    static telnetpp::byte_storage const expected_result = {
        telnetpp::iac, telnetpp::dont, 0xAB
    };

    ASSERT_EQ(expected_result, channel_.written_);
    ASSERT_TRUE(received_content_.empty());
    ASSERT_TRUE(complete_);
}

TEST_F(a_session, sends_dont_for_unrouted_wont)
{
    static telnetpp::byte_storage const content = {
        telnetpp::iac, telnetpp::wont, 0xAB
    };

    async_read();
    channel_.receive(content);

    static telnetpp::byte_storage const expected_result = {
        telnetpp::iac, telnetpp::dont, 0xAB
    };

    ASSERT_EQ(expected_result, channel_.written_);
    ASSERT_TRUE(received_content_.empty());
    ASSERT_TRUE(complete_);
}

TEST_F(a_session, sends_wont_for_unrouted_do)
{
    static telnetpp::byte_storage const content = {
        telnetpp::iac, telnetpp::do_, 0xAB
    };

    async_read();
    channel_.receive(content);

    static telnetpp::byte_storage const expected_result = {
        telnetpp::iac, telnetpp::wont, 0xAB
    };

    ASSERT_EQ(expected_result, channel_.written_);
    ASSERT_TRUE(received_content_.empty());
    ASSERT_TRUE(complete_);
}

TEST_F(a_session, sends_wont_for_unrouted_dont)
{
    static telnetpp::byte_storage const content = {
        telnetpp::iac, telnetpp::dont, 0xAB
    };

    async_read();
    channel_.receive(content);

    static telnetpp::byte_storage const expected_result = {
        telnetpp::iac, telnetpp::wont, 0xAB
    };

    ASSERT_EQ(expected_result, channel_.written_);
    ASSERT_TRUE(received_content_.empty());
    ASSERT_TRUE(complete_);
}

TEST_F(a_session, can_send_commands)
{
    session_.write(telnetpp::command(telnetpp::ayt));

    telnetpp::byte_storage const expected_result = {
        telnetpp::iac, telnetpp::ayt
    };

    ASSERT_EQ(expected_result, channel_.written_);
}

TEST_F(a_session, can_send_plain_text)
{
    static telnetpp::byte_storage const content = {
        'T', 'E', 'S', 'T', 0xFF
    };

    session_.write(content);

    static telnetpp::byte_storage const expected_result = {
        'T', 'E', 'S', 'T', 0xFF, 0xFF
    };

    ASSERT_EQ(expected_result, channel_.written_);
}

TEST_F(a_session, can_receive_data_piecemeal)
{
    static telnetpp::option_type const client_option = 0xD0;
    static telnetpp::option_type const server_option = 0xD7;

    fake_client_option client{session_, client_option};
    fake_server_option server{session_, server_option};

    telnetpp::byte_storage client_subnegotiation_content;
    client.on_subnegotiation.connect(
        [&](telnetpp::bytes content) {
            client_subnegotiation_content.append(
                content.begin(),
                content.end());
        });

    telnetpp::byte_storage server_subnegotiation_content;
    server.on_subnegotiation.connect(
        [&](telnetpp::bytes content) {
            server_subnegotiation_content.append(
                content.begin(),
                content.end());
        });

    session_.install(client);
    session_.install(server);

    static auto const content = 
        "HELLO"
        "\xFF\xFB\xD0"
        "WORLD" "\xFF\xFF"
        "\xFF\xFD\xD7"
        "\xFF\xFA\xD7" "TEST" "\xFF\xF0"_tb;

    for (auto const ch : content)
    {
        async_read();

        telnetpp::bytes const byte_content{ &ch, 1 };
        channel_.receive(byte_content);

        ASSERT_TRUE(std::exchange(complete_, false));
    }

    static auto const expected_content = "HELLOWORLD" "\xFF"_tb;
    static auto const expected_result =
        "\xFF\xFD\xD0"
        "\xFF\xFB\xD7"_tb;
    static auto const expected_server_subnegotiation_content = "TEST"_tb;
    static auto const expected_client_subnegotiation_content = ""_tb;

    ASSERT_EQ(expected_content, received_content_);
    ASSERT_EQ(expected_result, channel_.written_);
    ASSERT_EQ(expected_client_subnegotiation_content, client_subnegotiation_content);
    ASSERT_EQ(expected_server_subnegotiation_content, server_subnegotiation_content);

    ASSERT_TRUE(client.active());
    ASSERT_TRUE(server.active());
}
