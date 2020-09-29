#pragma once
#include <mutex>
#include <vector>
#include <queue>
#include <memory>
#include <list>
#include <functional>
#include "object_pool.h"
#include <iostream>
#include <streambuf>

namespace bcore {
	const uint32_t log2Table[32] = { 0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3 };
	inline uint32_t Log2Int(uint32_t x, bool& has_left) {
		uint32_t ans = 0;
		auto srcx = x;
		if (x >= 65536) { ans += 16; x >>= 16; }
		if (x >= 256) { ans += 8; x >>= 8; }
		if (x >= 16) { ans += 4; x >>= 4; }
		auto n = ans + log2Table[x];
		has_left = (1 << n) != srcx;
		return n;
	}

	class Slice {
	public:
		Slice() {}
		Slice(char* data_arg, uint32_t cap_size, uint32_t len) : data_(data_arg), cap_(cap_size), len_(len) {}
		char* data() {
			return data_;
		}
		inline char* writable_data() {
			return data_ + len_;
		}
		inline uint32_t writable_size() {
			return cap_ - len_;
		}
		uint32_t cap() {
			return cap_;
		}

		explicit operator bool() const noexcept {
			return cap_ > 0;
		}
		uint32_t len() {
			return len_;
		}
		bool append(const char* str, uint32_t size) {
			if (size <= cap_ && len_ <= cap_ - size) {
				memcpy(data_ + len_, str, size);
				len_ += size;
				return true;
			}
			return false;
		}
		bool append(char c) {
			if (len_ < cap_) {
				data_[len_++] = c;
				return true;
			}
			return false;
		}
		void add_len(uint32_t add) {
			len_ += add;
			if (len_ > cap_) {
				len_ = cap_;
			}
		}
		void reset_len(uint32_t new_len) {
			len_ = new_len > cap_ ? cap_ : new_len;
		}
		void reset_data(char* data_arg, uint32_t len_arg) {
			data_ = data_arg;
			len_ = len_arg;
			cap_ = len_;
		}
		char get(uint32_t index) {
			return data_[index];
		}
		void write_buffer(char* ptr, uint32_t size) {
			auto real_size = std::min(size, cap_ - len_);
			memcpy(data_ + len_, ptr, real_size);
			len_ += real_size;
		}
		inline Slice to_slice() {
			return Slice(data_, cap_, len_);
		}
		Slice make_slice(uint32_t start_index, uint32_t end_index) {
			if (start_index > len_) {
				start_index = len_;
			}
			if (end_index < start_index) {
				end_index = len_;
			}
			else if (end_index < start_index) {
				end_index = start_index;
			}
			
			auto temp_cap = end_index - start_index;
			return Slice(data_ + start_index, temp_cap, temp_cap);
		}
	protected:
		char* data_;
		uint32_t cap_;
		uint32_t len_;
	};
	//128M 64M 32M 16M 8M 4M 2M 1M 512K 256K 128K 64K
	//
#ifdef BTEST
	class ByteBuf;
	typedef std::function<void(ByteBuf*, bool)> AllocType;
	extern AllocType ByteBufAlloc;
	static void SetByteBufAlloc(AllocType f) {
		ByteBufAlloc = f;
	}
#endif
	class ByteBuf : public Slice {
	public:
		ByteBuf(uint32_t cap_arg) : Slice(new char[cap_arg], cap_arg, 0) {
#ifdef BTEST
			if (ByteBufAlloc != nullptr) {
				ByteBufAlloc(this, true);
			}
#endif
		}
		~ByteBuf() {
			cap_ = 0;
			delete[] data_;
			data_ = nullptr;
#ifdef BTEST
			if (ByteBufAlloc != nullptr) {
				ByteBufAlloc(this, false);
			}
#endif
		}
		void reset_data(char* data_arg, uint32_t len_arg) = delete;
		Slice to_slice() {
			return Slice(data_, cap_, 0);
		}
	};
	using SliceSharedPtr = std::shared_ptr<Slice>;
	static void ResetByteBuf(ByteBuf* buf) {
		buf->reset_len(0);
	}
	using DeleterType = std::function<void(ByteBuf*)>;
	using UniqueByteBuf = std::unique_ptr<ByteBuf, DeleterType>;
	using SharedByteBuf = std::shared_ptr<ByteBuf>;

	class ByteHelper {
	public:
		static void WriteData(ByteBuf* buf, void* ptr, uint32_t size) {
			
		}
	};


	class SectionBuffer {
	public:
		SectionBuffer() : buffer_size_(0) {}
		SectionBuffer(uint32_t buffer_size, int64_t pool_check_interval) :
			buffer_size_(buffer_size) {
			pool_ = ObjectPool<ByteBuf>::NewPool(pool_check_interval, ResetByteBuf);
		}
		UniqueByteBuf AllocBuffer() {
			return pool_->GetUniqueObject(buffer_size_);
		}
		SharedByteBuf AllocSharedBuffer() {
			return pool_->GetSharedObject(buffer_size_);
		}
	private:
		uint32_t buffer_size_;
		std::shared_ptr<ObjectPool<ByteBuf>> pool_;
	};

	class BufferPool : public Singleton<BufferPool> {
	public:
		BufferPool();
		static UniqueByteBuf AllocBuffer(uint32_t min_size) {
			return Instance().rawAllocBuffer(min_size);
		}
		static SharedByteBuf AllocSharedBuffer(uint32_t min_size) {
			return Instance().rawAllocSharedBuffer(min_size);
		}
	protected:
		UniqueByteBuf rawAllocBuffer(uint32_t min_size);
		SharedByteBuf rawAllocSharedBuffer(uint32_t min_size);

	private:
		std::vector<SectionBuffer> buffer_pool_;
		//uint32_t max_index_;
		//uint32_t min_index_;
		uint32_t max_buffer_size_;
		uint32_t min_buffer_size_;
	};
}