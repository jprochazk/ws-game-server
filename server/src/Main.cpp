
#pragma warning(disable:4275)

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/version.hpp>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <spdlog/spdlog.h>
#include <mutex>
#include <optional>
#include <chrono>

#include "network/socket_listener.h"
#include "world/World.h"

int main(int argc, char* argv[])
{
	const char* address = "127.0.0.1";
	const unsigned short port = 8001;

	spdlog::info("Starting server @127.0.0.1:8001");

	auto network_thread = std::thread([&] {
		boost::asio::io_context ioc;
		std::make_shared<socket_listener>(ioc,
			boost::asio::ip::tcp::endpoint{
				boost::asio::ip::make_address(address),
				port
			},
			World::Instance()->GetSocketManager()
		)->run();
		// start the io context on the network thread
		ioc.run();
	});

	auto last_update = std::chrono::steady_clock::now();
	for (;;) {
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last_update);
		if (elapsed.count() >= 1000) {
			last_update = std::chrono::steady_clock::now();
			World::Instance()->Update();
		}
	}

	network_thread.join();

	return EXIT_SUCCESS;
}

