#pragma once

#include <memory>
#include <functional>
#include "bcore/buffer_pool.h"
#include "net_interface.h"

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
			for (auto& it : session_ops_) {
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
		}
	private:
		std::vector<SessionOptionFunc> session_ops_;
	};
	SessionOptionFunc set_read_buffer_size(uint32_t read_buffer_size) {
		return [](SessionOption& option) {
			option.read_buffer_size = read_buffer_size;
		};
	}
	SessionOptionFunc set_write_buffer_size(uint32_t write_buffer_size)  {
		return [](SessionOption& option) {
			option.write_buffer_size = write_buffer_size;
		};
	}
	SessionOptionFunc set_message_handler(std::shared_ptr<IMessageHandler> handler) {
		return [](SessionOption& option) {
			option.handler_ = std::move(handler);
		};
	}

	struct ClientOption;
	using ClientOptionFunc = std::function<void(ClientOption& option)>;
	struct ClientOption : SessionOption {
		std::string connect_ip;
		int connect_port = 0;
		std::string client_name;

		void StartUp(ErrorCode& err) {
			SessionOption::StartUp(err);
			for (auto& it : opts) {
				it(*this);
			}
			opts.clear();
		}
		void SetClientOption(std::initializer_list<ClientOptionFunc> funcs) {
			for (auto& it : funcs) {
				opts.emplace_back(std::move(it));
			}
		}
	private:
		std::vector<ClientOptionFunc> opts;
	};

	struct ServerOption;
	using ServerOptionFunc = std::function<(ServerOption&)>;
	struct ServerOption : SessionOption {
		std::string listen_ip;
		int listen_port = 0;
		std::string server_name;

		void SetServerOption(std::initializer_list<ServerOptionFunc> funcs) {
			for (auto& it : funcs) {
				server_option_opts.emplace_back(std::move(it));
			}
		}
		void StartUp(ErrorCode& err) {
			SessionOption::StartUp(err);
			for (auto& it : server_option_opts) {
				it(*this):
			}
			server_option_opts.clear();
		}
	private:
		std::vector<ServerOptionFunc> server_option_opts;
	};
}