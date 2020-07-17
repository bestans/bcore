#pragma once
#include <mutex>
#include <vector>
#include <queue>
#include <memory>
#include <list>
#include <functional>

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

template <class T>
struct MoveWraper {
	T value;

	MoveWraper(const MoveWraper& v) : value(std::move(v)) {}
};

//128M 64M 32M 16M 8M 4M 2M 1M 512K 256K 128K 64K
//

class ByteBuf {
public:
	ByteBuf(uint32_t size) {

	}
};

template <class T>
class SimpleStack {
public:
	typedef std::function<std::unique_ptr<T>()> CreateFunc;
	SimpleStack(uint32_t capacity, CreateFunc create_func) :
		capacity_(capacity), len_(0), data_list_(4),
		create_func_(std::move(create_func))
	{
	}
	~SimpleStack() {
		for (auto& it : data_list_) {
			if (it != nullptr) {
				delete it;
			}
		}
		data_list_.clear();
	}
	std::unique_ptr<T> Pop() {
		if (len_ > 0) {
			len_--;
			auto ptr = data_list_[len_];
			data_list_[len_] = nullptr;
			return std::unique_ptr<T>(ptr);
		}
		return std::move(create_func_());
	}
	void Push(std::unique_ptr<T> data) {
		if (!data) {
			return;
		}
		if (len_ < data_list_.size()) {
			data_list_[len_++] = data.release();
		}
		else if (data_list_.size() < capacity_) {
			data_list_.resize(std::min(uint32_t(data_list_.size() * 2), capacity_));
			data_list_[len_++] = data.release();
		}
	}
private:
	uint32_t capacity_;
	uint32_t len_;
	std::vector<T*> data_list_;
	CreateFunc create_func_;
};

template <class T>
class SectionBuffer {
public:
	SectionBuffer(uint32_t max_count, uint32_t buffer_size) :
		max_count_(max_count),
		buffer_size_(buffer_size),
		mutex_(new std::mutex()),
		data_list_(max_count, [=]() { return std::make_unique<T>(buffer_size); }) {

	}
	SectionBuffer() : SectionBuffer(0, 0) {

	}
	std::unique_ptr<T> GetBuffer() {
		std::lock_guard<std::mutex> lock(*mutex_);
		return std::move(data_list_.Pop());
	}
	void RecycleBuffer(std::unique_ptr<T> buffer) {
		std::lock_guard<std::mutex> lock(*mutex_);
		data_list_.Push(std::move(buffer));
	}
private:
	std::mutex* mutex_;
	uint32_t buffer_size_;
	uint32_t max_count_;
	SimpleStack<T> data_list_;
};
template <class T>
class BufferPool {
public:
	BufferPool(uint32_t max_index, uint32_t min_index) : max_index_(max_index), min_index_(min_index) {
		if (min_index < 1)
			throw new std::exception("invalid min_index:must >= 1");
		if (max_index >= 32 || max_index <= min_index)
			throw new std::exception("invalid max_index:must < 32 && > min_index");

		max_buffer_size_ = 1 << max_index;
		min_buffer_size_ = (1 << min_index) + 1;
		auto cur_index = 0;
		for (uint32_t i = min_index + 1; i <= max_index; i++) {
			auto buffer_size = 1 << i;
			buffer_pool_.emplace_back(std::move(SectionBuffer<T>(max_buffer_size_ / buffer_size, buffer_size)));
		}
	}
	std::unique_ptr<T> GetBuffer(uint32_t min_size) {
		if (min_size < min_buffer_size_) {
			return std::make_unique<T>(min_size);
		}
		bool has_left = false;
		auto index = Log2Int(min_size, has_left);
		if (has_left) index++;
		if (index > min_index_ && index <= max_index_) {
			return std::move(buffer_pool_[index - min_index_ - 1].GetBuffer());
		}
		return std::move(std::make_unique<T>(min_size));
	}
	void RecycleBuffer(std::unique_ptr<T> buffer) {
		if (buffer->GetSize() < min_buffer_size_) {
			return;
		}
		bool has_left = false;
		auto index = Log2Int(buffer->GetSize(), has_left);
		if (has_left) index++;
		if (index > min_index_ && index <= max_index_) {
			return buffer_pool_[index - 1 - min_index_].RecycleBuffer(std::move(buffer));
		}
	}

private:
	std::vector<SectionBuffer<T>> buffer_pool_;
	uint32_t max_index_;
	uint32_t min_index_;
	uint32_t max_buffer_size_;
	uint32_t min_buffer_size_;
};
