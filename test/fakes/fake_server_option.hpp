#pragma once

#include <telnetpp/server_option.hpp>

class fake_server_option : public telnetpp::server_option
{
public:
    explicit fake_server_option(telnetpp::session &sess, telnetpp::option_type option)
      : telnetpp::server_option(sess, option)
    {
    }

    boost::signals2::signal<
        void (telnetpp::bytes data)
    > on_subnegotiation;

private:
    void handle_subnegotiation(telnetpp::bytes data) override
    {
        on_subnegotiation(data);
    }
};
