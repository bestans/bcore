#include "buffer_pool.h"
#include <gtest/gtest.h>
using namespace std;

TEST(testCase, Log2Int) {
	bool has_left = false;
	EXPECT_EQ(Log2Int(2, has_left), 0) << "xxs";
}

class TestBuffer {
public:
	static uint32_t alloc, free_size;
	TestBuffer(uint32_t size) {
		size_ = size;
		alloc += size;
	}
	~TestBuffer() {
		free_size += size_;
	}
	uint32_t size_;
	uint32_t GetSize() const {
		return size_;
	}
};
uint32_t TestBuffer::alloc = 0;
uint32_t TestBuffer::free_size = 0;

TEST(testCase, bufferpool) {
	{
		BufferPoolTest<TestBuffer> pool(10, 2);
		for (int i = 0; i < 1000; i++) {
			auto buffer = pool.GetBuffer(i);
			pool.RecycleBuffer(std::move(buffer));
		}
	}
	auto v = new char[0];
	cout << "bufferpool:" << TestBuffer::alloc << "," << TestBuffer::free_size << "," << (v == nullptr) << endl;
}
