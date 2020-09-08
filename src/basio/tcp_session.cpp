#include "basio/tcp_session.h"
#include "bcore/buffer_pool.h"

using namespace bcore;
namespace bcore_basio {
	void TcpSession::DoRead()
	{
		auto self = shared_from_this();
		socket_.async_read_some(
			asio::buffer(read_buffer_->writable_data(), read_buffer_->writable_size()), [this, self](asio::error_code ec, std::size_t bytes_transferred)
			{
				if (ec) {
					return;
				}
				//socket_.shutdown(asio::socket_base::shutdown_both);
				read_buffer_->add_len(bytes_transferred);
				bnet::ErrorCode err;
				bcore::Slice readable;
				while (true) {
					readable.reset_data(read_buffer_->readable_data(), read_buffer_->readable_size());
					auto read_len = message_handler_->DecodeMessage(this, readable, err);
					if (err) {
						return;
					}
					if (read_len <= 0) {
						break;
					}
					read_buffer_->add_read_index(read_len);
					if (read_buffer_->readable_size() <= 0) {
						break;
					}
				}
				if (read_buffer_->readable_size() <= 0) {
					read_buffer_->reset();
				}
				if (read_buffer_->readable_size() > 0) {
					if (read_buffer_->readable_size() >= read_buffer_->cap()) {
						//buffer已经满了，解析部分数据
						readable.reset_data(read_buffer_->readable_data(), read_buffer_->readable_size());
						auto msg_info = message_handler_->DecodePartMessage(this, readable, err);
						//close session
						return;
					}
					read_buffer_bak_->write_buffer(read_buffer_->readable_data(), read_buffer_->readable_size());
					read_buffer_->reset();
				}
				else {
					read_buffer_.swap(read_buffer_bak_);
				}
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

	void TcpSession::SendMessage(void* message) {
		bnet::ErrorCode err;
		auto buffer = message_handler_->EncodeMessage(this, message, err);
		if (err) {
			std::cout << "SendMessage failed:" << err.message() << std::endl;
			return;
		}
		WriteBuffer(std::move(buffer));
	}
}