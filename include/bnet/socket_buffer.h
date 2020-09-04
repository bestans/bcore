#pragma once
#include <memory>
#include "bcore/buffer_pool.h"

namespace bnet {
	class SocketBuffer {
	public:
		SocketBuffer(uint32_t size) : read_index_(0) {
			buffer_ = bcore::BufferPool::AllocBuffer(size);
		}
		uint32_t cap() {
			return buffer_->cap();
		}
		uint32_t add_len(uint32_t write_size) {
			buffer_->add_len(write_size);
		}
		inline uint32_t len() {
			return buffer_->len();
		}
		void add_read_index(uint32_t read_size) {
			read_index_ += read_size;
			if (read_index_ > len()) {
				read_index_ = len();
			}
		}
		char* readable_data() {
			return buffer_->data() + readable_size();
		}
		uint32_t readable_size() {
			return len() - read_index_;
		}
		inline char* writable_data() {
			return buffer_->data() + len();
		}
		uint32_t writable_size() {
			return buffer_->writable_size();
		}
		void reset() {
			read_index_ = 0;
			buffer_->reset_len(0);
		}
		void write_buffer(char* data, uint32_t size) {
			buffer_->write_buffer(data, size);
		}
	private:
		bcore::UniqueByteBuf buffer_;
		uint32_t read_index_;
	};
	using SocketBufferSharedPtr = std::shared_ptr<SocketBuffer>;
	using SocketBufferUniquePtr = std::unique_ptr<SocketBuffer>;
}
