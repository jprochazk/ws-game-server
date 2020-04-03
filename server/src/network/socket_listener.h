
#pragma once
#ifndef SOCKET_LISTENER_H
#define SOCKET_LISTENER_H

#include <boost/asio/strand.hpp>
#include <boost/beast.hpp>

#include "websocket.h"
#include "socket_manager.h"

namespace beast = boost::beast;

class socket_listener : public std::enable_shared_from_this<socket_listener>
{
	boost::asio::io_context& ioc_;
	boost::asio::ip::tcp::acceptor acceptor_;
	socket_manager* smgr_;

public:
	socket_listener(
		boost::asio::io_context& ioc,
		boost::asio::ip::tcp::endpoint endpoint,
		socket_manager* socket_manager
	);

	void run();
private:
	void on_accept(beast::error_code ec, boost::asio::ip::tcp::socket socket);
};

#endif SOCKET_LISTENER_H