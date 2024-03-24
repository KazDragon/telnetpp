#include "telnet_option_fixture.hpp"

#include <gtest/gtest.h>
#include <telnetpp/detail/overloaded.hpp>
#include <telnetpp/options/mccp/codec.hpp>
#include <telnetpp/options/mccp/server.hpp>

using namespace telnetpp::literals;  // NOLINT

namespace {

// Reversible "compression" function.
template <typename Continuation>
void compress_decompress(telnetpp::bytes data, Continuation &&continuation)
{
    std::vector<telnetpp::byte> converted_data(data.size());
    std::transform(
        data.begin(), data.end(), converted_data.begin(), [](auto by) {
            return by ^ 0x20;
        });
    continuation(converted_data);
}

class fake_compressor : public telnetpp::options::mccp::codec
{
public:
    bool end_compression_next_chunk_ = false;
    bool finished_ = false;

private:
    telnetpp::bytes transform_chunk(
        telnetpp::bytes data, continuation const &cont) override
    {
        compress_decompress(data, [&](auto const &data) {
            cont(data, end_compression_next_chunk_);
        });

        return telnetpp::bytes{};
    }

    void do_finish(continuation const &cont) override
    {
        auto const marker = "end"_tb;
        cont(marker, true);
        finished_ = true;
    }
};

class an_mccp_server : public a_telnet_option_base
{
protected:
    void send_data(telnetpp::bytes data)
    {
        compressor_(
            data,
            [this](telnetpp::bytes compressed_data, bool compression_end) {
                session_.write(compressed_data);
                compression_ended_ = compression_end;
            });
    }

    fake_compressor compressor_;
    telnetpp::options::mccp::server server_{session_, compressor_};

    bool compression_ended_ = false;
};

}  // namespace

TEST_F(an_mccp_server, reports_mccp_option_code)
{
    ASSERT_EQ(86, server_.option_code());
}

TEST_F(an_mccp_server, sends_nothing_when_being_activated_remotely)
{
    // In particular, we mean apart from our confirmation of the
    // negotiation.
    server_.negotiate(telnetpp::do_);
    assert(server_.active());

    telnetpp::byte_storage const expected_data = {
        telnetpp::iac, telnetpp::will, server_.option_code()};

    ASSERT_EQ(expected_data, channel_.written_);
}

namespace {

class an_active_mccp_server : public an_mccp_server
{
protected:
    an_active_mccp_server()
    {
        server_.negotiate(telnetpp::do_);
        assert(server_.active());
        channel_.written_.clear();
    }
};

}  // namespace

TEST_F(
    an_active_mccp_server,
    sends_empty_subnegotiation_when_beginning_compression)
{
    server_.start_compression();

    telnetpp::byte_storage const expected = {
        telnetpp::iac,
        telnetpp::sb,
        server_.option_code(),
        telnetpp::iac,
        telnetpp::se};

    ASSERT_EQ(expected, channel_.written_);
}

TEST_F(an_active_mccp_server, sends_nothing_when_finishing_compression)
{
    server_.finish_compression();

    telnetpp::byte_storage const expected = {};
    ASSERT_EQ(expected, channel_.written_);
}

TEST_F(an_active_mccp_server, sends_data_uncompressed)
{
    telnetpp::byte_storage const test_data = "test_data"_tb;
    send_data(test_data);

    telnetpp::byte_storage const expected_data = {
        test_data.begin(), test_data.end()};

    ASSERT_EQ(expected_data, channel_.written_);
}

namespace {

class an_mccp_server_activated_with_compression_started
  : public an_active_mccp_server
{
protected:
    an_mccp_server_activated_with_compression_started()
    {
        server_.start_compression();
        channel_.written_.clear();
    }
};

}  // namespace

TEST_F(an_mccp_server_activated_with_compression_started, sends_compressed_data)
{
    telnetpp::byte_storage const test_data = "test_data"_tb;
    send_data(test_data);

    telnetpp::byte_storage const expected_data = [&]() {
        telnetpp::byte_storage data;

        compress_decompress(test_data, [&](telnetpp::bytes bytes) {
            data.insert(data.end(), bytes.begin(), bytes.end());
        });

        return data;
    }();

    ASSERT_EQ(expected_data, channel_.written_);
}

TEST_F(
    an_mccp_server_activated_with_compression_started,
    sends_compression_end_compressed_when_compression_is_ended)
{
    server_.finish_compression();

    telnetpp::byte_storage const expected_data = "end"_tb;

    ASSERT_EQ(expected_data, channel_.written_);
    ASSERT_TRUE(compressor_.finished_);
}

TEST_F(
    an_mccp_server_activated_with_compression_started,
    sends_compression_end_compressed_and_negotiation_uncompressed)
{
    server_.negotiate(telnetpp::dont);

    telnetpp::byte_storage const expected_data = {
        'e', 'n', 'd', telnetpp::iac, telnetpp::wont, server_.option_code()};

    ASSERT_EQ(expected_data, channel_.written_);
    ASSERT_TRUE(compressor_.finished_);
}
