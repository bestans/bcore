#include <iostream>
#include <deque>
#include <thread>
#include <mutex>
#include <chrono>
#include "bcore.hpp"
#include "gtest/gtest.h"
#include <atomic>

using namespace std;
using namespace bcore;

void test_dataqueue() {
	auto beginTime = std::chrono::high_resolution_clock::now();
	DataQueue<int> dataq;
	int maxC = 100000;
	int send_t_count = 10;
	int total_send_times = maxC * send_t_count;
	atomic<int> real_total_send_times = 0;
	vector<thread> threads; 
	for (int i = 0; i < send_t_count; i++) {
		threads.emplace_back([&] {
			int i = 0;
			while (++i <= maxC) {
				if (dataq.Push(int(1))) {
					real_total_send_times++;
				}
			}
			if (real_total_send_times >= total_send_times) {
				dataq.Stop();
			}
			});
	}
	std::atomic<int> receiveC = 0;
	int multiC = 0;
	size_t multiTimes = 0;
	for (int i = 0; i < 5; i++) {
		threads.emplace_back([&] {
			std::deque<int> in;
			while (dataq.Pop(in)) {
				while (!in.empty()) {
					receiveC++;
					in.pop_front();
				}
			}
			});
	}
	for (int i = 0; i < 5; i++) {
		threads.emplace_back([&] {
			int value = 0;
			while (dataq.Pop(value)) {
				receiveC++;
			}
			});
	}
	for (auto& t : threads) {
		t.join();
	}
	auto endTime = std::chrono::high_resolution_clock::now();
	auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime);
	std::cout << "elapsed time is " << elapsedTime.count() << " milliseconds" << "," << total_send_times << "," << multiTimes << "," << receiveC << std::endl;
}

TEST(data_queue, test_threads) {
	test_dataqueue();
}
