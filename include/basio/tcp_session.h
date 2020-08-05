#include "asio.hpp"
#include <array>
#include "thread_pool.h"

using asio::ip::tcp;
namespace bcore_basio {
	class TcpSession : public std::enable_shared_from_this<TcpSession>
	{
	public:
		TcpSession(std::shared_ptr<ThreadPoolContext> threadCtx) :
			thread_context_(threadCtx),
			socket_(*(thread_context_->ctx)),
			strand_(*(thread_context_->ctx))
		{ }

		tcp::socket& Socket() { return socket_; }
		void Start() { DoRead(); }
	private:
		void DoRead();
		void DoWrite(std::size_t length);
	private:
		std::shared_ptr<ThreadPoolContext> thread_context_;
		tcp::socket socket_;
		asio::io_context::strand strand_;
		std::array<char, 8192> buffer_;
	};
}