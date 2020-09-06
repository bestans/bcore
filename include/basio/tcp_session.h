#include "asio.hpp"
#include <array>
#include <queue>
#include <atomic>
#include "thread_pool.h"
#include "bcore/buffer_pool.h"
#include "bnet/socket_buffer.h"
#include "bnet/msghandler/message_handler.h"
#include "bnet/tcp_option.h"
#include "bnet/socket_buffer.h"

using asio::ip::tcp;
namespace bcore_basio {
	class TcpSession : public std::enable_shared_from_this<TcpSession>, public bnet::ISession
	{
	public:
		TcpSession(std::shared_ptr<ThreadPoolContext> threadCtx, std::shared_ptr<bnet::SessionOption> option) :
			thread_context_(threadCtx),
			socket_(*(thread_context_->ctx)),
			option_(std::move(option)),
			message_handler_(option_->handler)
		{
			read_buffer_ = std::make_unique<bnet::SocketBuffer>(option_->read_buffer_size);
			read_buffer_bak_ = std::make_unique<bnet::SocketBuffer>(option_->read_buffer_size);
		}

		tcp::socket& Socket() { return socket_; }
		void Start() { DoRead(); }
		void WriteBuffer(bcore::UniqueByteBuf buf) {
			bool can_write = false;
			{
				std::lock_guard<std::mutex> lock(write_buffer_mutex_);
				write_buffer_list_.emplace(std::move(buf));
				if (!write_flag_) {
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
		std::shared_ptr<bnet::SessionOption> option_;
		std::shared_ptr<bnet::IMessageHandler> message_handler_;

		std::mutex write_buffer_mutex_;

		std::queue<bcore::UniqueByteBuf> writing_buffer_list_;
		std::queue<bcore::UniqueByteBuf> write_buffer_list_;
		std::atomic_bool write_flag_;

		bnet::SocketBufferUniquePtr read_buffer_;
		bnet::SocketBufferUniquePtr read_buffer_bak_;
	};
}