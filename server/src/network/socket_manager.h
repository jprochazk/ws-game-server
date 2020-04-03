#pragma once

#ifndef SOCKET_MANAGER_H
#define SOCKET_MANAGER_H

#include <atomic>
#include <unordered_map>
#include <mutex>
#include <algorithm>

#include "websocket.h"

class socket_manager {
public:
	void add_socket(std::shared_ptr<websocket> socket);
	void close_socket(uint16_t id);
	void update();
protected: // callbacks
	virtual void on_add_socket(uint16_t socket_id, std::shared_ptr<websocket> socket) = 0;
	virtual void on_close_socket(uint16_t socket_id, std::shared_ptr<websocket> socket) = 0;
private: // internal state
	std::mutex new_sockets_lock_;
	std::vector<std::shared_ptr<websocket>> new_sockets_;
	std::unordered_map<uint16_t, std::shared_ptr<websocket>> sockets_;
	std::atomic<uint16_t> id_sequence_;
}; // class socket_manager

#endif // SOCKET_MANAGER_H