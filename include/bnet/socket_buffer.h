#pragma once
#include <memory>
#include "bcore/buffer_pool.h"

namespace bnet {
	class SocketBuffer {
	public:
		SocketBuffer(uint32_t size) : write_index_(0), read_index_(0) {
			buffer_ = bcore::BufferPool::AllocBuffer(size);
		}
		uint32_t cap() {
			return buffer_->cap();
		}
		void add_write_index(uint32_t write_size) {
			write_index_ += write_size;
			if (write_index_ > cap()) {
				write_index_ = cap();
			}
		}
		void add_read_index(uint32_t read_size) {
			read_index_ += read_size;
			if (read_index_ > write_index_) {
				read_index_ = write_index_;
			}
		}
		char* readable_data() {
			return buffer_->data() + readable_bytes();
		}
		uint32_t readable_bytes() {
			return write_index_ - read_index_;
		}
		char* writable_data() {
			return buffer_->data() + write_index_;
		}
		uint32_t writable_bytes() {
			return cap() - write_index_;
		}
	private:
		bcore::UniqueByteBuf buffer_;
		uint32_t write_index_;
		uint32_t read_index_;
	};
	using SocketBufferSharedPtr = std::shared_ptr<SocketBuffer>;
	using SocketBufferUniquePtr = std::unique_ptr<SocketBuffer>;
}
