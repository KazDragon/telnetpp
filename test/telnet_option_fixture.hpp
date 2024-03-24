#pragma once

#include "fakes/fake_channel.hpp"

#include <gtest/gtest.h>
#include <telnetpp/session.hpp>

class a_telnet_option_base : public testing::Test
{
protected:
    fake_channel channel_;
    telnetpp::session session_{channel_};
};

template <typename Option>
class a_telnet_option : public a_telnet_option_base
{
protected:
    Option option_{session_};
};
