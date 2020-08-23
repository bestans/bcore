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

TEST(buffer_pool, test) {
	{
		std::atomic<int> alloc_times, free_times;
		alloc_times = free_times = 0;
		SetByteBufAlloc([&](ByteBuf* buf, bool alloc) {
			alloc ? alloc_times++ : free_times++;
			});
		std::vector<std::thread> threads;
		for (int i = 0; i < 10; i++) {
			threads.emplace_back([]() {
				for (int j = 0; j < 1000; j++) {
					auto ptr = BufferPool::AllocBuffer(j);
					auto ptr2 = BufferPool::AllocSharedBuffer(j * 1000);
				}
				});
		}
		for (auto& t : threads) {
			t.join();
		}
		int times = 4;
		Timer::AddRepeatedTimer(50, times, [&]() {
			times--;
			BTime::SetOffset((int64_t)((10 - times) * 60000));
			});
		std::this_thread::sleep_for(std::chrono::milliseconds(400));
		SetByteBufAlloc(nullptr);
		BTime::SetOffset(0);
		cout << "alloc_times=" << alloc_times << ",free_times=" << free_times << endl;
		EXPECT_EQ(alloc_times, free_times) << "alloc free not equal\n";
	}
}

TEST(buffer_pool, test_size) {
	int max_size = 1 << 18;
	std::cout << "buffer_pool:[" << (1 << 2 + 1) << "," << max_size << "]\n";
	for (int j = 1; j <= 19; j++) {
		int normal_size = 1 << j;
		int test_size[] = { normal_size, normal_size - 1, normal_size + 1 };
		for (auto size : test_size) {
			auto temp = BufferPool::AllocBuffer(size);
			auto temp_shared = BufferPool::AllocSharedBuffer(size);
			EXPECT_EQ(temp->GetSize(), temp_shared->GetSize());
			EXPECT_GE(temp->GetSize(), (uint32_t)size);
		}
		auto ptr = BufferPool::AllocBuffer(normal_size);
		auto ptr2 = BufferPool::AllocSharedBuffer(normal_size);
		EXPECT_EQ(ptr->GetSize(), ptr2->GetSize()) << "unique size != shared size";
		if (normal_size <= 5 || normal_size >= max_size) {
			EXPECT_EQ(ptr->GetSize(), normal_size) << "not in range size error";
			continue;
		}
		auto normal_ptr = BufferPool::AllocBuffer(normal_size);
		auto last_ptr = BufferPool::AllocBuffer(normal_size - 1);
		auto next_ptr = BufferPool::AllocBuffer(normal_size + 1);

		EXPECT_GE(normal_ptr->GetSize(), (uint32_t)normal_size);
		EXPECT_LE(last_ptr->GetSize(), (uint32_t)normal_size);
		EXPECT_EQ(next_ptr->GetSize(), (uint32_t)2 * (uint32_t)normal_size);
	}
}