
#pragma once
#ifndef SOCKET_MANAGER_H
#define SOCKET_MANAGER_H

#include <atomic>
#include <unordered_map>
#include <mutex>
#include <algorithm>
#include <spdlog/spdlog.h>

#include "websocket.h"

class socket_manager {
public:
	void add_socket(std::shared_ptr <websocket> socket);
	void close_socket(uint16_t id);
	void update_sockets();
	void process_packets();
	void broadcast(std::vector<uint8_t> payload);
protected:
	std::mutex sockets_lock_;
	std::unordered_map<uint16_t, std::shared_ptr<websocket>> sockets_;
	std::atomic<uint16_t> id_sequence_;
}; // class socket_manager

#endif // SOCKET_MANAGER_H