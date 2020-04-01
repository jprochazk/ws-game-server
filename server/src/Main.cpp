#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
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

#include "socket_listener.h"
#include "socket_manager.h"
#include "websocket.h"


class World : public socket_manager {
public:
	static World* instance()
	{
		static World instance;
		return &instance;
	}

	void update() {
		// clear closed connections
		update_sockets();
		process_packets();

		// TODO: update entities, scripts, ...
		spdlog::info("World::update, sockets: {}", sockets_.size());
	}

	// Send a message to every session
	void broadcast(std::vector<uint8_t> payload) {
		auto message = std::make_shared<std::vector<uint8_t>>(std::move(payload));
		std::lock_guard<std::mutex> lock(sockets_lock_);
		for (auto itr = sockets_.begin(); itr != sockets_.end(); itr++) {
			itr->second->send(*message);
		}
	}
private:
	explicit World() : socket_manager() {}
};

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
			World::instance() // using the world instance as the session manager
		)->run();
		// start the io context on the network thread
		ioc.run();
	});

	auto last_update = std::chrono::steady_clock::now();
	for (;;) {
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last_update);
		if (elapsed.count() >= 1000) {
			last_update = std::chrono::steady_clock::now();
			World::instance()->update();
		}
	}

	network_thread.join();

	return EXIT_SUCCESS;
}