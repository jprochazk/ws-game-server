
#pragma once
#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#pragma warning(disable:4275)

#include <functional>
#include <spdlog/spdlog.h>
#include <boost/asio/dispatch.hpp>
#include <boost/beast.hpp>

namespace beast = boost::beast;

class websocket : public std::enable_shared_from_this<websocket>
{
private:
	beast::websocket::stream<beast::tcp_stream> ws_;
	beast::flat_buffer buffer_;
	std::mutex write_lock_;
	std::vector<std::vector<uint8_t>> write_queue_;
	std::mutex recv_lock_;
	std::vector<std::vector<uint8_t>> recv_queue_;
	std::atomic<bool> is_writing_async_;

	std::atomic<bool> should_close_;
public:
	websocket(boost::asio::ip::tcp::socket&& socket);

	void run();
	void close(beast::websocket::close_reason cr = beast::websocket::close_code::normal);
	void send(std::vector<uint8_t> const message);
	std::vector<std::vector<uint8_t>> get_packets();
	bool should_close();

private:
	void on_accept(beast::error_code ec);
	void on_read(beast::error_code ec, std::size_t bytes_transferred);
	void on_send(std::vector<uint8_t> message);
	void on_write(beast::error_code ec, std::size_t bytes_transferred);
}; // class websocket

#endif // WEBSOCKET_H