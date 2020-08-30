#include "basio/tcp_session.h"
#include "bcore/buffer_pool.h"

using namespace bcore;
namespace bcore_basio {
	void TcpSession::DoRead()
	{
		auto self = shared_from_this();
		socket_.async_read_some(
			asio::buffer(read_buffer_->writable_data(), read_buffer_->writable_bytes()), [this, self](asio::error_code ec, std::size_t bytes_transferred)
			{
				read_buffer_->add_write_index(bytes_transferred);
				auto buf = BufferPool::AllocBuffer(5);
				buf->WriteBuffer(buffer_.data(), bytes_transferred);
				WriteBuffer(std::move(buf));
				DoRead();
			});
	}
	void TcpSession::DoWrite()
	{
		auto buf = std::move(writing_buffer_list_.front());
		writing_buffer_list_.pop();
		auto self = shared_from_this();
		auto data = buf->data();
		auto len = buf->len();
		asio::async_write(
			socket_, asio::buffer(data, len),
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
		});
	}
}