#include "expect_elements.hpp"
#include <telnetpp/options/mccp/codec.hpp>
#include "fakes/fake_compressor.hpp"
#include "fakes/fake_decompressor.hpp"
#include <gtest/gtest.h>
#include <memory>

TEST(mccp_codec_test, sending_empty_tokens_returns_empty_tokens)
{
    // In the case that we try to send an empty stream of tokens, there is no
    // need to do anything, and we shouldn't do anything.
    auto compressor   = std::make_shared<fake_compressor>();
    auto decompressor = std::make_shared<fake_decompressor>();
    
    telnetpp::options::mccp::codec codec{
        std::shared_ptr<telnetpp::options::mccp::compressor>(compressor),
        std::shared_ptr<telnetpp::options::mccp::decompressor>(decompressor)};
    
    auto const &expected = std::vector<telnetpp::stream_token>{};
    auto const &result = codec.send({});
    
    expect_tokens(expected, result);
    ASSERT_EQ(size_t{0}, compressor->compress_called);
    ASSERT_EQ(size_t{0}, compressor->end_compression_called);
    ASSERT_EQ(size_t{0}, decompressor->decompress_called);
}

TEST(mccp_codec_test, sending_uncompressed_data_returns_uncompressed_data)
{
    // In the case that compression has not been started, the codec should
    // return the data unchanged and not call the compressor at all.
    auto compressor   = std::make_shared<fake_compressor>();
    auto decompressor = std::make_shared<fake_decompressor>();
    
    telnetpp::options::mccp::codec codec{
        std::shared_ptr<telnetpp::options::mccp::compressor>(compressor),
        std::shared_ptr<telnetpp::options::mccp::decompressor>(decompressor)};

    auto const &data = telnetpp::u8stream {
        't', 'e', 's', 't', 'd', 'a', 't', 'a'
    };
    
    auto const &expected = std::vector<telnetpp::stream_token> { data };
    
    expect_tokens(expected, codec.send({ data }));
    ASSERT_EQ(size_t{0}, compressor->compress_called);
    ASSERT_EQ(size_t{0}, compressor->end_compression_called);
    ASSERT_EQ(size_t{0}, decompressor->decompress_called);
}