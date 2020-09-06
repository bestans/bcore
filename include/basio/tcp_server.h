#pragma once
#include "asio.hpp"
#include "thread_pool.h"
#include "bnet/tcp_option.h"

using asio::ip::tcp;
namespace bcore_basio {
	class TcpServer
	{
	public:
		TcpServer(std::shared_ptr<ThreadPoolContext> threadCtx, unsigned short port) :
			thread_context_(threadCtx),
			acceptor_(*(threadCtx->ctx), tcp::endpoint(tcp::v4(), port)),
			pool_(threadCtx->pool),
			option_(std::make_shared<bnet::ServerOption>())
		{
			DoAccept();
		}
		void DoAccept();

	private:
		std::shared_ptr<ThreadPoolContext> thread_context_;
		tcp::acceptor acceptor_;
		std::shared_ptr<ThreadPool> pool_;
		std::shared_ptr<bnet::ServerOption> option_;
	};
}
