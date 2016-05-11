#include "expect_elements.hpp"
#include <telnetpp/options/mccp/codec.hpp>
#include <telnetpp/options/mccp/mccp.hpp>
#include "fakes/fake_compressor.hpp"
#include "fakes/fake_decompressor.hpp"
#include <gtest/gtest.h>
#include <memory>

TEST(mccp_codec_test, sending_empty_tokens_returns_empty_tokens)
{
    // In the case that we try to send an empty stream of tokens, there is no
    // need to do anything, and we won't do anything.
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
    ASSERT_EQ(size_t{0}, decompressor->end_decompression_called);
}

TEST(mccp_codec_test, sending_uncompressed_data_returns_uncompressed_data)
{
    // In the case that compression has not been started, the codec will
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
    ASSERT_EQ(size_t{0}, decompressor->end_decompression_called);
}

TEST(mccp_codec_test, uncompressed_sending_any_passes_through_any)
{
    // If the datastream contains a token of type boost::any, and it doesn't
    // contain a token of the type that the codec consumes, then it will
    // be passed through unchanged.
    auto compressor   = std::make_shared<fake_compressor>();
    auto decompressor = std::make_shared<fake_decompressor>();

    telnetpp::options::mccp::codec codec{
        std::shared_ptr<telnetpp::options::mccp::compressor>(compressor),
        std::shared_ptr<telnetpp::options::mccp::decompressor>(decompressor)};

    struct pass_through {};

    auto const &expected = std::vector<telnetpp::stream_token> {
        boost::any(pass_through{})
    };

    expect_tokens(expected, codec.send({ boost::any(pass_through{}) }));
    ASSERT_EQ(size_t{0}, compressor->compress_called);
    ASSERT_EQ(size_t{0}, compressor->end_compression_called);
    ASSERT_EQ(size_t{0}, decompressor->decompress_called);
    ASSERT_EQ(size_t{0}, decompressor->end_decompression_called);
}

TEST(mccp_codec_test, uncompressed_sending_end_compression_returns_no_data)
{
    // In the uncompressed state, there is no compression to end.  So this
    // call will return no data and will not forward on to the compressor.
    auto compressor   = std::make_shared<fake_compressor>();
    auto decompressor = std::make_shared<fake_decompressor>();

    telnetpp::options::mccp::codec codec{
        std::shared_ptr<telnetpp::options::mccp::compressor>(compressor),
        std::shared_ptr<telnetpp::options::mccp::decompressor>(decompressor)};

    auto const &expected = std::vector<telnetpp::stream_token> {};

    expect_tokens(expected, codec.send({
        boost::any(telnetpp::options::mccp::end_compression{})
    }));
    ASSERT_EQ(size_t{0}, compressor->compress_called);
    ASSERT_EQ(size_t{0}, compressor->end_compression_called);
    ASSERT_EQ(size_t{0}, decompressor->decompress_called);
    ASSERT_EQ(size_t{0}, decompressor->end_decompression_called);
}

TEST(mccp_codec_test, uncompressed_begin_compression_returns_no_data)
{
    // In the uncompressed state, beginning compression has no immediate
    // effect.  No data will be returned and no functions on the compressor
    // will be called.
    auto compressor   = std::make_shared<fake_compressor>();
    auto decompressor = std::make_shared<fake_decompressor>();

    telnetpp::options::mccp::codec codec{
        std::shared_ptr<telnetpp::options::mccp::compressor>(compressor),
        std::shared_ptr<telnetpp::options::mccp::decompressor>(decompressor)};

    auto const &expected = std::vector<telnetpp::stream_token> {};

    expect_tokens(expected, codec.send({
        boost::any(telnetpp::options::mccp::begin_compression{})
    }));
    ASSERT_EQ(size_t{0}, compressor->compress_called);
    ASSERT_EQ(size_t{0}, compressor->end_compression_called);
    ASSERT_EQ(size_t{0}, decompressor->decompress_called);
    ASSERT_EQ(size_t{0}, decompressor->end_decompression_called);
}

TEST(mccp_codec_test, compression_begun_sending_data_returns_compressed_data)
{
    // In the compressed state, where the codec has received a
    // begin_compression token, any data that is sent will be put through
    // the compressor.
    auto compressor   = std::make_shared<fake_compressor>();
    compressor->compress_result = { 'c', 'o', 'm', 'p' };
    auto decompressor = std::make_shared<fake_decompressor>();

    telnetpp::options::mccp::codec codec{
        std::shared_ptr<telnetpp::options::mccp::compressor>(compressor),
        std::shared_ptr<telnetpp::options::mccp::decompressor>(decompressor)};

    auto const &expected = std::vector<telnetpp::stream_token> {
        telnetpp::u8stream { 'c', 'o', 'm', 'p' }
    };

    auto const &data = telnetpp::u8stream{ 'd', 'a', 't', 'a' };

    expect_tokens(expected, codec.send({
        boost::any(telnetpp::options::mccp::begin_compression{}),
        data
    }));
    ASSERT_EQ(size_t{1}, compressor->compress_called);
    ASSERT_EQ(size_t{0}, compressor->end_compression_called);
    ASSERT_EQ(size_t{0}, decompressor->decompress_called);
    ASSERT_EQ(size_t{0}, decompressor->end_decompression_called);
}

TEST(mccp_codec_test, compression_begun_sending_any_passes_through_any)
{
    // In the compressed state, just as with the compressed state,
    // if the datastream contains a token of type boost::any, and it doesn't
    // contain a token of the type that the codec consumes, then it will
    // be passed through unchanged.
    auto compressor   = std::make_shared<fake_compressor>();
    auto decompressor = std::make_shared<fake_decompressor>();

    telnetpp::options::mccp::codec codec{
        std::shared_ptr<telnetpp::options::mccp::compressor>(compressor),
        std::shared_ptr<telnetpp::options::mccp::decompressor>(decompressor)};

    struct pass_through {};

    auto const &expected = std::vector<telnetpp::stream_token> {
        boost::any(pass_through{})
    };

    auto const &data = std::vector<telnetpp::stream_token> {
        boost::any(telnetpp::options::mccp::begin_compression{}),
        boost::any(pass_through{})
    };

    expect_tokens(expected, codec.send(data));
    ASSERT_EQ(size_t{0}, compressor->compress_called);
    ASSERT_EQ(size_t{0}, compressor->end_compression_called);
    ASSERT_EQ(size_t{0}, decompressor->decompress_called);
    ASSERT_EQ(size_t{0}, decompressor->end_decompression_called);
}

TEST(mccp_codec_test, compressed_ending_compression_returns_end_compression_sequence)
{
    // In the compressed state, after end_compression has been called, the
    // codec should be un the uncompressed state - data is sent on without
    // being passed to the compressor.
    auto compressor   = std::make_shared<fake_compressor>();
    compressor->end_compression_result = { 'e', 'n', 'd' };
    compressor->compress_result = { 'c', 'o', 'm', 'p' };
    auto decompressor = std::make_shared<fake_decompressor>();

    telnetpp::options::mccp::codec codec{
        std::shared_ptr<telnetpp::options::mccp::compressor>(compressor),
        std::shared_ptr<telnetpp::options::mccp::decompressor>(decompressor)};

    auto const &expected = std::vector<telnetpp::stream_token> {
        telnetpp::u8stream { 'e', 'n', 'd' }, // TODO: coalesce u8streams?
        telnetpp::u8stream { 'd', 'a', 't', 'a' }
    };

    auto const &data = std::vector<telnetpp::stream_token> {
        boost::any(telnetpp::options::mccp::begin_compression{}),
        boost::any(telnetpp::options::mccp::end_compression{}),
        telnetpp::u8stream { 'd', 'a', 't', 'a' }
    };

    expect_tokens(expected, codec.send(data));
    ASSERT_EQ(size_t{0}, compressor->compress_called);
    ASSERT_EQ(size_t{1}, compressor->end_compression_called);
    ASSERT_EQ(size_t{0}, decompressor->decompress_called);
    ASSERT_EQ(size_t{0}, decompressor->end_decompression_called);
}

TEST(mccp_codec_test, compressed_ending_compression_then_sending_data_returns_uncompressed_data)
{
    auto compressor   = std::make_shared<fake_compressor>();
    compressor->end_compression_result = { 'e', 'n', 'd' };
    auto decompressor = std::make_shared<fake_decompressor>();

    telnetpp::options::mccp::codec codec{
        std::shared_ptr<telnetpp::options::mccp::compressor>(compressor),
        std::shared_ptr<telnetpp::options::mccp::decompressor>(decompressor)};

    auto const &expected = std::vector<telnetpp::stream_token> {
        telnetpp::u8stream { 'e', 'n', 'd' }
    };

    auto const &data = std::vector<telnetpp::stream_token> {
        boost::any(telnetpp::options::mccp::begin_compression{}),
        boost::any(telnetpp::options::mccp::end_compression{})
    };

    expect_tokens(expected, codec.send(data));
    ASSERT_EQ(size_t{0}, compressor->compress_called);
    ASSERT_EQ(size_t{1}, compressor->end_compression_called);
    ASSERT_EQ(size_t{0}, decompressor->decompress_called);
    ASSERT_EQ(size_t{0}, decompressor->end_decompression_called);
}

TEST(mccp_codec_test, uncompressed_receive_returns_data)
{
    // If we are not receiving any compressed data, then receiving data
    // via the codec should just return it as received and not pass it
    // to the decompressor.
    auto compressor   = std::make_shared<fake_compressor>();
    auto decompressor = std::make_shared<fake_decompressor>();

    telnetpp::options::mccp::codec codec{
        std::shared_ptr<telnetpp::options::mccp::compressor>(compressor),
        std::shared_ptr<telnetpp::options::mccp::decompressor>(decompressor)};


    std::vector<telnetpp::stream_token> expected {
        telnetpp::u8stream{'X'}
    };

    auto result = std::vector<telnetpp::stream_token> {
        codec.receive('X')
    };

    expect_tokens(expected, result);
    ASSERT_EQ(size_t{0}, compressor->compress_called);
    ASSERT_EQ(size_t{0}, compressor->end_compression_called);
    ASSERT_EQ(size_t{0}, decompressor->decompress_called);
    ASSERT_EQ(size_t{0}, decompressor->end_decompression_called);
}

TEST(mccp_codec_test, uncompressed_end_decompression_does_nothing)
{
    // If we are not currently receiving any compressed data, then ending
    // decompression does nothing.
    auto compressor   = std::make_shared<fake_compressor>();
    auto decompressor = std::make_shared<fake_decompressor>();

    telnetpp::options::mccp::codec codec{
        std::shared_ptr<telnetpp::options::mccp::compressor>(compressor),
        std::shared_ptr<telnetpp::options::mccp::decompressor>(decompressor)};

    auto const &expected = std::vector<telnetpp::stream_token> {};

    expect_tokens(expected, codec.send({
        boost::any(telnetpp::options::mccp::end_decompression{})
    }));
    ASSERT_EQ(size_t{0}, compressor->compress_called);
    ASSERT_EQ(size_t{0}, compressor->end_compression_called);
    ASSERT_EQ(size_t{0}, decompressor->decompress_called);
    ASSERT_EQ(size_t{0}, decompressor->end_decompression_called);
}

TEST(mccp_codec_test, uncompressed_sending_begin_decompression_returns_nothing)
{
    // Sending a begin_decompression should consume the token and return
    // nothing.  Any other effects occur later.
    auto compressor   = std::make_shared<fake_compressor>();
    auto decompressor = std::make_shared<fake_decompressor>();

    telnetpp::options::mccp::codec codec{
        std::shared_ptr<telnetpp::options::mccp::compressor>(compressor),
        std::shared_ptr<telnetpp::options::mccp::decompressor>(decompressor)};

    auto const &expected = std::vector<telnetpp::stream_token> {};

    expect_tokens(expected, codec.send({
        boost::any(telnetpp::options::mccp::begin_decompression{})
    }));
    ASSERT_EQ(size_t{0}, compressor->compress_called);
    ASSERT_EQ(size_t{0}, compressor->end_compression_called);
    ASSERT_EQ(size_t{0}, decompressor->decompress_called);
    ASSERT_EQ(size_t{0}, decompressor->end_decompression_called);
}

TEST(mccp_codec_test, compressed_received_data_is_decompressed)
{
    auto compressor   = std::make_shared<fake_compressor>();
    auto decompressor = std::make_shared<fake_decompressor>();
    decompressor->decompress_result.emplace_back(telnetpp::u8stream{}, false);
    decompressor->decompress_result.emplace_back(telnetpp::u8stream{}, false);
    decompressor->decompress_result.emplace_back(telnetpp::u8stream{}, false);
    decompressor->decompress_result.emplace_back(telnetpp::u8stream{ 'd', 'a', 't', 'a' }, false);

    telnetpp::options::mccp::codec codec{
        std::shared_ptr<telnetpp::options::mccp::compressor>(compressor),
        std::shared_ptr<telnetpp::options::mccp::decompressor>(decompressor)};

    codec.send({boost::any(telnetpp::options::mccp::begin_decompression{})});

    auto const &expected = std::vector<telnetpp::stream_token> {
        telnetpp::u8stream{},
        telnetpp::u8stream{},
        telnetpp::u8stream{},
        telnetpp::u8stream{'d', 'a', 't', 'a'},
    };

    auto const &data = telnetpp::u8stream{'c', 'o', 'm', 'p'};
    auto result = std::vector<telnetpp::stream_token>{};

    for (auto byte : data)
    {
        result.push_back(codec.receive(byte));
    }

    expect_tokens(expected, result);
    ASSERT_EQ(size_t{0}, compressor->compress_called);
    ASSERT_EQ(size_t{0}, compressor->end_compression_called);
    ASSERT_EQ(size_t{4}, decompressor->decompress_called);
    ASSERT_EQ(size_t{0}, decompressor->end_decompression_called);
}