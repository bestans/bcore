#include "asio.hpp"
#include <array>
#include <queue>
#include <atomic>
#include "thread_pool.h"
#include "bcore/buffer_pool.h"

using asio::ip::tcp;
namespace bcore_basio {
	class TcpSession : public std::enable_shared_from_this<TcpSession>
	{
	public:
		TcpSession(std::shared_ptr<ThreadPoolContext> threadCtx) :
			thread_context_(threadCtx),
			socket_(*(thread_context_->ctx)),
			strand_(*(thread_context_->ctx)),
			buffer_()
		{ }

		tcp::socket& Socket() { return socket_; }
		void Start() { DoRead(); }
		void WriteBuffer(bcore::SharedByteBuf buf) {
			bool can_write = false;
			{
				std::lock_guard<std::mutex> lock(write_buffer_mutex_);
				write_buffer_list_.push(std::move(buf));
				if (write_flag_.load()) {
					write_flag_ = true;
					can_write = true;
					std::swap(writing_buffer_list_, write_buffer_list_);
				}
			}
			if (can_write) {
				DoWrite();
			}
		}
	private:
		void DoRead();
		void DoWrite();
	private:
		std::shared_ptr<ThreadPoolContext> thread_context_;
		tcp::socket socket_;
		asio::io_context::strand strand_;
		std::array<char, 8192> buffer_;

		std::mutex write_buffer_mutex_;
		std::queue<bcore::SharedByteBuf> writing_buffer_list_;
		std::queue<bcore::SharedByteBuf> write_buffer_list_;
		std::atomic_bool write_flag_;
	};
}