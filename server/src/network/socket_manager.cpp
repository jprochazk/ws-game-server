#include "socket_manager.h"

void socket_manager::add_socket(std::shared_ptr<websocket> socket) 
{
	std::lock_guard<std::mutex> lock(new_sockets_lock_);
	new_sockets_.push_back(socket);
}

void socket_manager::close_socket(uint16_t id) {
	if (auto socket = sockets_.find(id)->second) {
		socket->close(beast::websocket::close_code::normal);
	}
	sockets_.erase(id);
}

void socket_manager::update()
{
	{
		std::lock_guard<std::mutex> lock(new_sockets_lock_);
		for (auto socket : new_sockets_) {
			socket->run();
			on_add_socket(id_sequence_, socket);
			sockets_.insert(std::make_pair(id_sequence_.load(), socket));
			++id_sequence_;
		}
		new_sockets_.erase(new_sockets_.begin(), new_sockets_.end());
	}

	auto itr = sockets_.begin();
	while (itr != sockets_.end()) {
		if (itr->second->should_close()) {
			itr->second->close(beast::websocket::close_code::normal);
			on_close_socket(itr->first, itr->second);
			itr = sockets_.erase(itr);
		}
		else {
			itr++;
		}
	}
}