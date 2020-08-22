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
		ByteBuf(uint32_t size) {
#ifdef BTEST
			if (ByteBufAlloc != nullptr) {
				ByteBufAlloc(this, true);
			}
#endif
		}
		~ByteBuf() {
#ifdef BTEST
			if (ByteBufAlloc != nullptr) {
				ByteBufAlloc(this, false);
			}
#endif
		}
	};

	//template <class T>
	//class SectionBuffer {
	//public:
	//	typedef std::function<void(T*)> DeleterType;
	//	SectionBuffer() {}
	//	SectionBuffer(uint32_t buffer_size) :
	//		buffer_size_(buffer_size) {
	//		pool_ = ObjectPool<T>::NewPool();
	//	}
	//	std::unique_ptr<T, DeleterType> AllocBuffer() {
	//		return pool_->GetUniqueObject(buffer_size_);
	//	}
	//private:
	//	uint32_t buffer_size_;
	//	std::shared_ptr<ObjectPool<T>> pool_;
	//};
	//template <class T>
	//class BufferPool {
	//public:
	//	typedef std::function<void(T*)> DeleterType;
	//	BufferPool(uint32_t max_index, uint32_t min_index) : max_index_(max_index), min_index_(min_index) {
	//		if (min_index < 1)
	//			throw new std::exception("invalid min_index:must >= 1");
	//		if (max_index >= 32 || max_index <= min_index)
	//			throw new std::exception("invalid max_index:must < 32 && > min_index");

	//		max_buffer_size_ = 1 << max_index;
	//		min_buffer_size_ = (1 << min_index) + 1;
	//		auto cur_index = 0;
	//		for (uint32_t i = min_index + 1; i <= max_index; i++) {
	//			auto buffer_size = 1 << i;
	//			buffer_pool_.emplace_back(std::move(SectionBuffer<T>(buffer_size)));
	//		}
	//	}
	//	std::unique_ptr<T, DeleterType> AllocBuffer(uint32_t min_size) {
	//		if (min_size < min_buffer_size_) {
	//			return ObjectPool<T>::GetUniqueObjectWithDefault(min_size);
	//		}
	//		bool has_left = false;
	//		auto index = Log2Int(min_size, has_left);
	//		if (has_left) index++;
	//		if (index > min_index_ && index <= max_index_) {
	//			return std::move(buffer_pool_[index - min_index_ - 1].AllocBuffer());
	//		}
	//		return ObjectPool<T>::GetUniqueObjectWithDefault(min_size);
	//	}

	//private:
	//	std::vector<SectionBuffer<T>> buffer_pool_;
	//	uint32_t max_index_;
	//	uint32_t min_index_;
	//	uint32_t max_buffer_size_;
	//	uint32_t min_buffer_size_;
	//};

	using DeleterType = std::function<void(ByteBuf*)>;
	using UniqueByteBuf = std::unique_ptr<ByteBuf, DeleterType>;
	using SharedByteBuf = std::shared_ptr<ByteBuf>;

	class SectionBuffer {
	public:
		SectionBuffer() {}
		SectionBuffer(uint32_t buffer_size, int64_t pool_check_interval) :
			buffer_size_(buffer_size) {
			pool_ = ObjectPool<ByteBuf>::NewPool(pool_check_interval);
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