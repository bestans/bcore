#pragma once
#include "asio.hpp"
#include "thread_pool.h"
#include "tcp_session.h"
#include "bnet/error_code.h"

using asio::ip::tcp;
namespace bcore_basio {
	class TcpClient
	{
	public:
		TcpClient(std::shared_ptr<ThreadPoolContext> threadCtx, unsigned short port) :
			thread_context_(threadCtx),
			pool_(threadCtx->pool)
		{
			session_ = std::make_shared<TcpSession>(pool_->AllocContext(1));
		}
		bnet::ErrorCode StartUp() {
			std::error_code err;
			tcp::resolver resolver(*(thread_context_->ctx));
			asio::connect(session_->Socket(), resolver.resolve(argv[1], argv[2]), err);
			if (!err) {
				return bnet::ErrorCode(bnet::ERROR_CODE::kConnectFailed, err.message());
			}
			session_->Start();
			return 0;
		}
		TcpSession* GetSession() {
			return session_.get();
		}
	private:
		std::shared_ptr<ThreadPoolContext> thread_context_;
		std::shared_ptr<ThreadPool> pool_;
		std::shared_ptr<TcpSession> session_;
	};
}