#include <gtest/gtest.h>
#include <telnetpp/options/mccp/codec.hpp>
#include <telnetpp/options/mccp/mccp.hpp>
#include "expect_elements.hpp"

TEST(mccp_codec_test, sending_empty_tokens_returns_empty_tokens)
{
    telnetpp::options::mccp::codec codec;

    auto tokens = std::vector<telnetpp::token>{};
    auto expected = std::vector<telnetpp::token>{};

    expect_tokens(expected, codec.send(tokens));
}

TEST(mccp_codec_test, uncompressed_sending_datastream_returns_datastream)
{
    telnetpp::options::mccp::codec codec;

    auto tokens = std::vector<telnetpp::token>{
        telnetpp::element{ "test" },
        telnetpp::element{ "datastream" }
    };

    auto expected = std::vector<telnetpp::token>{
        telnetpp::element{ "test" },
        telnetpp::element{ "datastream" }
    };

    expect_tokens(expected, codec.send(tokens));
}

TEST(mccp_codec_test, uncompressed_send_and_block_stops_data)
{
    telnetpp::options::mccp::codec codec;

    auto tokens = std::vector<telnetpp::token>{
        telnetpp::element{ "before" },
        boost::any(telnetpp::options::mccp::block_token{}),
        telnetpp::element{ "after" }
    };

    auto expected = std::vector<telnetpp::token>{
        telnetpp::element{ "before" }
    };

    expect_tokens(expected, codec.send(tokens));
}

TEST(mccp_codec_test, uncompressed_blocked_resume_uncompressed_returns_blocked_data)
{
    telnetpp::options::mccp::codec codec;

    auto tokens = std::vector<telnetpp::token>{
        telnetpp::element{ "before" },
        boost::any(telnetpp::options::mccp::block_token{}),
        telnetpp::element{ "after" }
    };

    codec.send(tokens);

    tokens = std::vector<telnetpp::token>{
        boost::any(telnetpp::options::mccp::resume_uncompressed_token{})
    };

    auto expected = std::vector<telnetpp::token>{
        telnetpp::element{ "after" }
    };

    expect_tokens(expected, codec.send(tokens));
}
