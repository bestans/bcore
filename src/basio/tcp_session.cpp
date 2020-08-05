#include "basio/tcp_session.h"

namespace bcore_basio {
	void TcpSession::DoRead()
	{
		auto self = shared_from_this();
		socket_.async_read_some(
			asio::buffer(buffer_, buffer_.size()),
			strand_.wrap([this, self](asio::error_code ec,
				std::size_t bytes_transferred)
				{
					if (!ec) { DoWrite(bytes_transferred); }
				}));
	}
	void TcpSession::DoWrite(std::size_t length)
	{
		auto self = shared_from_this();
		asio::async_write(
			socket_, asio::buffer(buffer_, length),
			strand_.wrap([this, self](asio::error_code ec,
				std::size_t /* bytes_transferred */)
				{
					if (!ec) { DoRead(); }
				}));
	}
}