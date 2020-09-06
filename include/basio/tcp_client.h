#pragma once
#include "asio.hpp"
#include "thread_pool.h"
#include "tcp_session.h"
#include "bnet/error_code.h"
#include "bnet/tcp_option.h"

using asio::ip::tcp;
namespace bcore_basio {
	class TcpClient
	{
	public:
		TcpClient(std::shared_ptr<ThreadPoolContext> threadCtx, std::shared_ptr<bnet::ClientOption> option) :
			thread_context_(threadCtx),
			pool_(threadCtx->pool),
			option_(std::move(option))
		{
			session_ = std::make_shared<TcpSession>(pool_->AllocContext(1), option_);
		}
		bnet::ErrorCode StartUp() {
			std::error_code err;
			bnet::ErrorCode err_code;
			if (!option_->StartUp(err_code)) {
				return std::move(err_code);
			}
			session_->Socket().connect(tcp::endpoint(asio::ip::address::from_string(option_->connect_ip), (unsigned short)option_->connect_port), err);
			if (!err) {
				return bnet::ErrorCode(bnet::ERROR_CODE::kConnectFailed, err.message());
			}
			session_->Start();
			return 0;
		}
		TcpSession* GetSession() {
			return session_.get();
		}
		bnet::ClientOption* GetOption() {
			return option_.get();
		}
	private:
		std::shared_ptr<ThreadPoolContext> thread_context_;
		std::shared_ptr<ThreadPool> pool_;
		std::shared_ptr<TcpSession> session_;
		std::shared_ptr<bnet::ClientOption> option_;
	};
}