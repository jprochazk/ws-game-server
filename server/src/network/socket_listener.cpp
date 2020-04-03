
#include "socket_listener.h"

socket_listener::socket_listener(
	boost::asio::io_context& ioc,
	boost::asio::ip::tcp::endpoint endpoint,
	socket_manager* socket_manager
) : ioc_(ioc), acceptor_(ioc), smgr_(socket_manager)
{
	beast::error_code ec;

	acceptor_.open(endpoint.protocol(), ec);
	if (ec) {
		spdlog::error("{}: {}", "open", ec.message());
		return;
	}

	acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
	if (ec) {
		spdlog::error("{}: {}", "set_option", ec.message());
		return;
	}

	acceptor_.bind(endpoint, ec);
	if (ec) {
		spdlog::error("{}: {}", "bind", ec.message());
		return;
	}

	acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
	if (ec) {
		spdlog::error("{}: {}", "listen", ec.message());
		return;
	}
}

void socket_listener::run()
{
	acceptor_.async_accept(
		boost::asio::make_strand(ioc_),
		beast::bind_front_handler(&socket_listener::on_accept, shared_from_this())
	);
}

void socket_listener::on_accept(
	beast::error_code ec, 
	boost::asio::ip::tcp::socket socket
) 
{
	if (ec) {
		spdlog::error("{}: {}", "accept", ec.message());
	}
	else {
		// give socket to socket manager
		smgr_->add_socket(std::make_shared<websocket>(std::move(socket)));
	}

	// Accept another connection
	acceptor_.async_accept(
		boost::asio::make_strand(ioc_),
		beast::bind_front_handler(&socket_listener::on_accept, shared_from_this())
	);
}