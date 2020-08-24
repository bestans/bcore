#pragma once
#include <mutex>
#include <vector>
#include <queue>
#include <memory>
#include <list>
#include <functional>
#include "object_pool.h"
#include <iostream>

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
	class ByteBuf {
	public:
		ByteBuf(uint32_t cap) : cap_(cap), len_(0) {
			buffer_ = new char[cap];
#ifdef BTEST
			if (ByteBufAlloc != nullptr) {
				ByteBufAlloc(this, true);
			}
#endif
		}
		~ByteBuf() {
			cap_ = 0;
			delete[] buffer_;
			buffer_ = nullptr;
#ifdef BTEST
			if (ByteBufAlloc != nullptr) {
				ByteBufAlloc(this, false);
			}
#endif
		}
		char* Data() {
			return buffer_;
		}
		uint32_t Cap() {
			return cap_;
		}
		uint32_t Len() {
			return len_;
		}
		void WriteBuffer(void *ptr, uint32_t size) {
			auto real_size = std::min(size, cap_ - len_);
			memcpy(buffer_ + len_, ptr, real_size);
			len_ += real_size;
		}
		inline void AddLen(uint32_t add) {
			len_ += add;
			if (len_ > cap_) {
				len_ = cap_;
			}
		}
		inline void ResetLen() {
			len_ = 0;
		}
	private:
		uint32_t cap_;
		uint32_t len_;
		char* buffer_;
	};
	static void ResetByteBuf(ByteBuf* buf) {
		buf->ResetLen();
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
		static inline UniqueByteBuf AllocBuffer(uint32_t min_size) {
			return Instance().rawAllocBuffer(min_size);
		}
		static inline SharedByteBuf AllocSharedBuffer(uint32_t min_size) {
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