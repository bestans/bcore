#pragma once

#include <memory>
#include <functional>
#include "bcore/buffer_pool.h"
#include "net_interface.h"

namespace bnet {
	const uint32_t kDefaultReadBufferSize = 65535;
	const uint32_t kDefaultWriteBufferSize = kDefaultReadBufferSize;
	const uint32_t 
	struct SessionOption {
		uint32_t read_buffer_size = kDefaultReadBufferSize;
		uint32_t write_buffer_size = kDefaultWriteBufferSize;
		uint32_t max_message_size = 0;
		IMessageHandler handler_ = nullptr;
	};
	using std::function<void(SessionOption& option)> SessionOptionFunc;
	SessionOptionFunc set_read_buffer_size(uint32_t read_buffer_size) {
		return [](SessionOption& option) {
			option.read_buffer_size = read_buffer_size;
		}
	}
	SessionOptionFunc set_write_buffer_size(uint32_t write_buffer_size)  {
		return [](SessionOption& option) {
			option.write_buffer_size = write_buffer_size;
		}
	}
	class SessionOptionImpl {
	public:
		inline void set_read_buffer_size(uint32_t read_buffer_size) {
			option_.read_buffer_size = read_buffer_size;
			option_.
		}
	protected:
		SessionOption option_;
	};
}