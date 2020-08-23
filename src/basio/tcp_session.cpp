#include "basio/tcp_session.h"

namespace bcore_basio {
	void TcpSession::DoRead()
	{
		auto self = shared_from_this();
		socket_.async_read_some(
			asio::buffer(buffer_, buffer_.size()),
			[this, self](asio::error_code ec,
				std::size_t bytes_transferred)
				{
					if (!ec) { DoWrite(); }
				});
	}
	void TcpSession::DoWrite()
	{
		auto buf = std::move(writing_buffer_list_.front());
		writing_buffer_list_.pop();
		auto self = shared_from_this();
		asio::async_write(
			socket_, asio::buffer(buf->GetBuffer(), buf->GetSize()),
			[this, self, hold = std::move(buf)](asio::error_code ec, std::size_t /* bytes_transferred */) {
			if (ec) {
				return;
			}
			if (!writing_buffer_list_.empty()) {
				DoWrite();
				return;
			}
			bool write_continue = false;
			{
				write_flag_ = false;
				std::lock_guard<std::mutex> lock(write_buffer_mutex_);
				if (write_buffer_list_.empty()) {
					return;
				}
				if (!write_flag_) {
					write_flag_ = true;
					write_continue = true;
					std::swap(writing_buffer_list_, write_buffer_list_);
				}
			}
			if (write_continue) {
				DoWrite();
			}
		}
	}
}