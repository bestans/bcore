#pragma once

#include <memory>
#include <functional>
#include <initializer_list>
#include "bcore/buffer_pool.h"
#include "net_interface.h"
#include "bnet/msghandler/message_handler.h"

namespace bnet {
	const uint32_t kDefaultReadBufferSize = 65535;
	const uint32_t kDefaultWriteBufferSize = kDefaultReadBufferSize;
	const uint32_t kMaxMessageSize = 65535;
	
	struct SessionOption;
	using SessionOptionFunc = std::function<void(SessionOption&)>;
	struct SessionOption {
		uint32_t read_buffer_size = kDefaultReadBufferSize;
		uint32_t write_buffer_size = kDefaultWriteBufferSize;
		uint32_t max_message_size = kMaxMessageSize;
		std::shared_ptr<IMessageHandler> handler;

		void SetSessionOption(std::initializer_list<SessionOptionFunc> funcs) {
			for (auto& it : funcs) {
				session_ops_.emplace_back(std::move(it));
			}
		}
		void StartUp(ErrorCode& err) {
			if (!handler) {
				handler = MessageHandler::Instance();
			}
			for (auto& it : session_ops_) {
				it(*this);
			}
			session_ops_.clear();
			handler->Init();
		}
		static inline SessionOptionFunc SetReadBufferSize(uint32_t read_buffer_size) {
			return [=](SessionOption& option) {
				option.read_buffer_size = read_buffer_size;
			};
		}
		static inline SessionOptionFunc SetWriteBufferSize(uint32_t write_buffer_size) {
			return [=](SessionOption& option) {
				option.write_buffer_size = write_buffer_size;
			};
		}
		static inline SessionOptionFunc SetMessageHandler(std::shared_ptr<IMessageHandler> handler) {
			return[arg_handler = std::move(handler)](SessionOption& option) {
				option.handler = arg_handler;
			};
		}
		static inline SessionOptionFunc SetReceiveMessageFunc(bnet::ReceiveMessageFunc func) {
			return[func_arg=std::move(func)](SessionOption& option) {
				option.handler->SetReceiveMessageFunc(func_arg);
			};
		}
	private:
		std::vector<SessionOptionFunc> session_ops_;
	};

	struct ClientOption;
	using ClientOptionFunc = std::function<void(ClientOption& option)>;
	struct ClientOption : SessionOption {
		std::string connect_ip = "127.0.0.1";
		int connect_port = 7777;
		std::string client_name;

		bool StartUp(ErrorCode& err) {
			for (auto& it : opts) {
				it(*this);
			}
			opts.clear();
			SessionOption::StartUp(err);
			return true;
		}
		void SetClientOption(std::initializer_list<ClientOptionFunc> funcs) {
			for (auto& it : funcs) {
				opts.push_back(it);
			}
		}
		static inline ClientOptionFunc SetConnectIp(std::string ip) {
			return [&](ClientOption& option) {
				option.connect_ip = std::move(ip);
			};
		}
		static inline ClientOptionFunc SetConnectPort(int port) {
			return[=](ClientOption& option) {
				option.connect_port = port;
			};
		}
	private:
		std::vector<ClientOptionFunc> opts;
	};

	struct ServerOption;
	using ServerOptionFunc = std::function<void(ServerOption&)>;
	struct ServerOption : SessionOption {
		std::string listen_ip = "127.0.0.1";
		int listen_port = 7777;
		std::string server_name;

		void SetServerOption(std::initializer_list<ServerOptionFunc> funcs) {
			for (auto& it : funcs) {
				server_option_opts.push_back(std::move(it));
			}
		}
		bool StartUp(ErrorCode& err) {
			for (auto& it : server_option_opts) {
				it(*this);
			}
			server_option_opts.clear();
			SessionOption::StartUp(err);
			return true;
		}
	private:
		std::vector<ServerOptionFunc> server_option_opts;
	};
}