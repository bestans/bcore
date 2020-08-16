#include "bcore.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <atomic>
#include "bcore/bcore_util.h"
#include "bcore/buffer_pool.h"
using namespace bcore;
using namespace std;
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
//TEST(bufferpool, test_memory) {
//	TestBuffer::alloc = 0;
//	TestBuffer::free_size = 0;
//	{
//		std::vector<std::thread> threads;
//		BufferPoolTest<TestBuffer> pool(10, 2, 10);
//		for (int i = 0; i < 11; i++) {
//			threads.emplace_back([&]() {
//				for (int i = 0; i < 100000; i++) {
//					auto buffer = pool.AllocBuffer(100);
//				}
//				});
//		}
//		std::this_thread::sleep_for(std::chrono::milliseconds(500));
//		for (int i = 0; i < 11; i++) {
//			threads.emplace_back([&]() {
//				for (int i = 0; i < 100000; i++) {
//					auto buffer = pool.AllocBuffer(i);
//				}
//				});
//		}
//		std::this_thread::sleep_for(std::chrono::milliseconds(500));
//		for (auto& t : threads) {
//			t.join();
//		}
//		EXPECT_EQ(TestBuffer::alloc, TestBuffer::free_size) << "alloc == free";
//	}
//	cout << "test_memory:alloc_size=" << TestBuffer::alloc << ",free_size=" << TestBuffer::free_size << endl;
//}
//
//class TestByteBuffer {
//public:
//	TestByteBuffer(uint32_t size) {
//		ptr = new char[size];
//	}
//	~TestByteBuffer() {
//		delete[] ptr;
//	}
//	char* ptr = nullptr;
//};
//TEST(bufferpool, test_memory2) {
//	BufferPoolTest<TestByteBuffer> pool(10, 2, 1000);
//	{
//		StopWatch watch;
//		std::vector<std::thread> threads;
//		for (int i = 0; i < 10; i++) {
//			threads.emplace_back([&]() {
//				for (int i = 0; i < 100000; i++) {
//					auto buffer = pool.AllocBuffer(10);
//				}
//				});
//		}
//		for (auto& t : threads) {
//			t.join();
//		}
//		std::cout << "cost miliseconds=" << watch.CostMilliSeconds() << std::endl;
//	}
//	{
//		StopWatch watch;
//		std::vector<std::thread> threads;
//		for (int i = 0; i < 10; i++) {
//			threads.emplace_back([&]() {
//				for (int i = 0; i < 100000; i++) {
//					auto buffer = std::make_shared<TestByteBuffer>(10);
//				}
//				});
//		}
//		for (auto& t : threads) {
//			t.join();
//		}
//		std::cout << "cost2 miliseconds=" << watch.CostMilliSeconds() << std::endl;
//	}
//	TestCoreLib();
//}

TEST(buffer_pool, test) {
	{

		SetByteBufAlloc([](ByteBuf* buf, bool alloc) {
			if (alloc) {
				cout << "construct1111111111111\n";
			}
			else {
				cout << "destruct11111111111111\n";
			}
		});
		auto buffer = BufferPool::AllocBuffers(111);
	}

}