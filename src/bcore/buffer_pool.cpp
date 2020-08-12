#include "bcore/buffer_pool.h"

namespace bcore {
	const uint32_t BufferPoolMinIndex = 2;
	const uint32_t BufferPoolMaxIndex = 10;
	const uint32_t BufferPoolCheckInterval = 60000;
	BufferPool::BufferPool() {
			max_buffer_size_ = 1 << BufferPoolMaxIndex;
			min_buffer_size_ = (1 << BufferPoolMinIndex) + 1;
			auto cur_index = 0;
			for (uint32_t i = BufferPoolMinIndex + 1; i <= BufferPoolMaxIndex; i++) {
				auto buffer_size = 1 << i;
				buffer_pool_.emplace_back(std::move(SectionBuffer(buffer_size, pool_check_interval)));
			}
	}
	UniqueByteBuf BufferPool::AllocBuffer(uint32_t min_size) {
		if (min_size < min_buffer_size_) {
			return ObjectPool<ByteBuf>::GetUniqueObjectWithDefault(min_size);
		}
		bool has_left = false;
		auto index = Log2Int(min_size, has_left);
		if (has_left) index++;
		if (index > BufferPoolMinIndex && index <= BufferPoolMaxIndex) {
			return std::move(buffer_pool_[index - min_index_ - 1].AllocBuffer());
		}
		return ObjectPool<ByteBuf>::GetUniqueObjectWithDefault(min_size);
	}
	SharedByteBuf BufferPool::AllocSharedBuffer(uint32_t min_size) {
		if (min_size < min_buffer_size_) {
			return std::make_shared(min_size);
		}
		bool has_left = false;
		auto index = Log2Int(min_size, has_left);
		if (has_left) index++;
		if (index > BufferPoolMinIndex && index <= BufferPoolMaxIndex) {
			return std::move(buffer_pool_[index - min_index_ - 1].AllocSharedBuffer());
		}
		return std::make_shared(min_size);
	}
}