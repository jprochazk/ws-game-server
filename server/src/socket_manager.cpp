#include "socket_manager.h"

void socket_manager::add_socket(std::shared_ptr<websocket> socket) 
{
	std::lock_guard<std::mutex> lock(sockets_lock_);
	socket->run();
	sockets_.insert(std::make_pair(id_sequence_++, socket));
}

void socket_manager::close_socket(uint16_t id) {
	std::lock_guard<std::mutex> lock(sockets_lock_);
	if (auto socket = sockets_[id]) {
		socket->close(beast::websocket::close_code::normal);
	}
	sockets_.erase(id);
}

void socket_manager::update_sockets()
{
	std::lock_guard<std::mutex> lock(sockets_lock_);
	auto itr = sockets_.begin();
	while (itr != sockets_.end()) {
		if (itr->second->should_close()) {
			itr->second->close(beast::websocket::close_code::normal);
			itr = sockets_.erase(itr);
		}
		else {
			itr++;
		}
	}
}

void socket_manager::process_packets() 
{
	std::lock_guard<std::mutex> lock(sockets_lock_);
	for (auto itr = sockets_.begin(); itr != sockets_.end(); itr++) {
		auto received_packets = itr->second->get_packets();
		for (auto& packet : received_packets) {
			// @temp only log the size of packet and which socket it came from
			spdlog::info("received data {} from socket num {}", packet.size(), itr->first);
		}
	}
}