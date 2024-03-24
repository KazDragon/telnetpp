#pragma once

#include <telnetpp/client_option.hpp>

class fake_client_option : public telnetpp::client_option
{
public:
    explicit fake_client_option(
        telnetpp::session &sess, telnetpp::option_type option)
      : telnetpp::client_option(sess, option)
    {
    }

    boost::signals2::signal<void(telnetpp::bytes data)> on_subnegotiation;

private:
    void handle_subnegotiation(telnetpp::bytes data) override
    {
        on_subnegotiation(data);
    }
};
