#include "asio.hpp"
#include <array>
#include "thread_pool.h"

using asio::ip::tcp;
namespace bcore_basio {
	class TcpSession : public std::enable_shared_from_this<TcpSession>
	{
	public:
		TcpSession(asio::io_context& io_context, ThreadPool& pool)
			: socket_(io_context),
			strand_(io_context),
			pool_(pool)
		{ }

		tcp::socket& Socket() { return socket_; }
		void Start() { DoRead(); }
	private:
		void DoRead();
		void DoWrite(std::size_t length);
	private:
		tcp::socket socket_;
		asio::io_context::strand strand_;
		std::array<char, 8192> buffer_;
		ThreadPool& pool_;
	};
}