#include "bcore.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <atomic>
using namespace std;
using namespace bcore;
TEST(bufferpool, Log2Int) {
	bool has_left = false;
	EXPECT_EQ(Log2Int(2, has_left), 1) << "Log2Int(2) == 1";
}

class TestBuffer {
public:
	static std::atomic<uint32_t> alloc, free_size;
	static uint32_t alloc11;
	static uint32_t free_size11;
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
std::atomic<uint32_t> TestBuffer::alloc = 0;
std::atomic<uint32_t> TestBuffer::free_size = 0;
uint32_t TestBuffer::alloc11 = 0;
uint32_t TestBuffer::free_size11 = 0;
TEST(bufferpool, test_memory) {
	TestBuffer::alloc = 0;
	TestBuffer::free_size = 0;
	{
		std::vector<std::thread> threads;
		BufferPool<TestBuffer> pool(10, 2);
		for (int i = 0; i < 11; i++) {
			threads.emplace_back([&]() {
				for (int i = 0; i < 100000; i++) {
					auto buffer = pool.AllocBuffer(100);
				}
				});
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		for (auto& t : threads) {
			t.join();
		}
	}
	EXPECT_EQ(TestBuffer::alloc, TestBuffer::free_size) << "alloc == free";
	cout << "test_memory:alloc_size=" << TestBuffer::alloc << ",free_size=" << TestBuffer::free_size << endl;
}
