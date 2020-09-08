#pragma once
#include "asio.hpp"
#include "thread_pool.h"
#include "bnet/tcp_option.h"

using asio::ip::tcp;
namespace bcore_basio {
	class TcpServer
	{
	public:
		TcpServer(std::shared_ptr<ThreadPoolContext> threadCtx, std::shared_ptr<bnet::ServerOption> option) :
			thread_context_(threadCtx),
			acceptor_(*(threadCtx->ctx), tcp::endpoint(asio::ip::address::from_string(option->listen_ip), option->listen_port)),
			pool_(threadCtx->pool),
			option_(std::move(option))
		{
		}
		bnet::ErrorCode StartUp() {
			std::error_code err;
			bnet::ErrorCode err_code;
			if (!option_->StartUp(err_code)) {
				return std::move(err_code);
			}
			DoAccept();
			return 0;
		}
		void DoAccept();

	private:
		std::shared_ptr<ThreadPoolContext> thread_context_;
		tcp::acceptor acceptor_;
		std::shared_ptr<ThreadPool> pool_;
		std::shared_ptr<bnet::ServerOption> option_;
	};
}
