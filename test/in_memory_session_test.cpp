#include <telnetpp/session.hpp>
#include <telnetpp/options/naws/client.hpp>
#include <gtest/gtest.h>

TEST(an_in_memory_session, may_respond_to_a_subnegotiation_with_a_negotiation)
{
	// A white-box test.
	// It is plausible that an in-memory session is responding directly to
	// a subnegotiation with some negotiation.  This may also be responded to.
	// It is necessary to keep the buffers in order during this process so that
	// the subnegotiation is not parsed multiple times.
	telnetpp::options::naws::client naws_client;
	telnetpp::session local_session;
	telnetpp::session remote_session;

	local_session.install(naws_client);
	naws_client.activate([](auto &&){});

	auto const &send_telnet_data = 
		[&](telnetpp::element const &elem)
		{
			remote_session.send(
				elem,
				[&](telnetpp::bytes data)
			{
				local_session.receive(
					data,
					[](telnetpp::element const &, auto &&) {},
					[](telnetpp::bytes) {});
			});
		};

	send_telnet_data(telnetpp::negotiation{ telnetpp::will, naws_client.option_code() });
	ASSERT_TRUE(naws_client.active());

	// Here, in response to receiving a window size update, we also receive more data.
	// as if we had sent a response in some way and were receiving the further response
	// synchronously.  In this sequence, the on_window_size_changed call should be 
	// executed exactly once.
	bool executed = false;
	naws_client.on_window_size_changed.connect(
		[&](auto, auto, auto &&)
		{
			ASSERT_FALSE(std::exchange(executed, true));
			send_telnet_data(telnetpp::negotiation{telnetpp::do_, naws_client.option_code()});
		});

	constexpr telnetpp::byte window_size_data[] = { 0, 0, 0, 0 };
	send_telnet_data(telnetpp::subnegotiation{ naws_client.option_code(), window_size_data });

	ASSERT_TRUE(executed);
}