
#include "websocket.h"

websocket::websocket(
	boost::asio::ip::tcp::socket&& socket
) : ws_(std::move(socket)),
	write_lock_(), write_queue_(), is_writing_async_(false),
	recv_lock_(), recv_queue_(),
	should_close_(false)
{
	ws_.binary(true);
}

void websocket::run()
{
	if (should_close_) return;
	ws_.set_option(
		beast::websocket::stream_base::timeout::suggested(beast::role_type::server)
	);
	ws_.set_option(
		beast::websocket::stream_base::decorator(
			[](beast::websocket::response_type& res) {
				res.set(beast::http::field::server, "rpg server");
			}
		)
	);
	ws_.async_accept(
		beast::bind_front_handler(&websocket::on_accept, shared_from_this())
	);
}

void websocket::close(beast::websocket::close_reason cr)
{
	should_close_ = true;
	if (ws_.is_open()) {
		ws_.async_close(cr, beast::bind_front_handler([&](beast::error_code ec) {
			if (ec) {
				spdlog::error("{}: {}", "close", ec.message());
			}
		}));
	}
}

void websocket::send(
	std::vector<uint8_t> const message
) {
	if (should_close_) return;
	write_queue_.push_back(message);

	boost::asio::post(
		ws_.get_executor(),
		beast::bind_front_handler(&websocket::on_send, shared_from_this())
	);
}

std::vector<std::vector<uint8_t>> websocket::get_packets() 
{
	if (should_close_) return {};
	std::unique_lock<std::mutex> recv_lock(recv_lock_);

	std::vector<std::vector<uint8_t>> buffer(
		std::make_move_iterator(recv_queue_.begin()),
		std::make_move_iterator(recv_queue_.end())
	);
	recv_queue_.erase(recv_queue_.begin(), recv_queue_.end());
	return buffer;
}

void websocket::on_accept(beast::error_code ec)
{
	if (should_close_) return;
	if (ec) {
		spdlog::error("{}: {}", "accept", ec.message());
		should_close_ = true;
		return;
	}

	ws_.async_read(
		buffer_,
		beast::bind_front_handler(&websocket::on_read, shared_from_this())
	);
}

void websocket::on_read(
	beast::error_code ec,
	std::size_t bytes_transferred
) {
	boost::ignore_unused(bytes_transferred);
	if (should_close_) return;

	if (ec) {
		if (ec != beast::websocket::error::closed) {
			spdlog::error("{}: {}", "read", ec.message());
		}
		should_close_ = true;
		return;
	}

	{
		std::unique_lock<std::mutex> lock(recv_lock_);

		std::vector<unsigned char> recv(buffer_.size());
		boost::asio::buffer_copy(
			boost::asio::buffer(recv.data(), buffer_.size()),
			buffer_.data()
		);

		recv_queue_.push_back(recv);
	}
	buffer_.consume(buffer_.size());

	ws_.async_read(
		buffer_,
		beast::bind_front_handler(&websocket::on_read, shared_from_this())
	);
}

void websocket::on_send() 
{
	if (should_close_) return;
	if (is_writing_async_) return;

	is_writing_async_ = true;

	ws_.async_write(
		boost::asio::buffer(write_queue_.front()),
		beast::bind_front_handler(&websocket::on_write, shared_from_this())
	);
}

void websocket::on_write(
	beast::error_code ec,
	std::size_t bytes_transferred
) {
	if (should_close_) return;
	if (ec) {
		spdlog::error("{}: {}", "write", ec.message());
		should_close_ = true;
		return;
	}

	write_queue_.erase(write_queue_.begin());

	if (!write_queue_.empty()) {
		ws_.async_write(
			boost::asio::buffer(write_queue_.front()),
			beast::bind_front_handler(
				&websocket::on_write,
				shared_from_this())
			);
	}
	else {
		is_writing_async_ = false;
	}
}

bool websocket::should_close() {
	return should_close_;
}