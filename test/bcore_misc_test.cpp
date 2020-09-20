#include "bcore.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <atomic>
#include "bcore/data_struct/stat_balance.h"

using namespace std;
using namespace bcore;
namespace test {

	class TestSingleton : public Singleton<TestSingleton> {
	public:
		int value = 0;
		TestSingleton() {
			value = 100;
		}
	};

	TEST(bcore_misc, singleton) {
		EXPECT_EQ(TestSingleton::Instance().value, 100);
	}
	TEST(bcore_misc, shared_ptr_virtual) {
		{
			//std::shared_ptr<ITimer> ptr1;
			//{
			//	std::shared_ptr<ITimer> vv = std::make_shared<Timer1>();
			//	ptr1 = vv;
			//}
			//cout << (dynamic_cast<TestSingleton*>(ptra) == nullptr) << endl;
		}
	}
	TEST(bcore_misc, data_heap) {
		DataHeap<int> heap;
		
		std::srand(time(0));
		for (int i = 0; i < 10; i++) {
			heap.Push(std::rand() % 100);
		}
		int value = 0;
		while (heap.Pop(value)) {
			cout << value << " ";
		}
		cout << endl;
	}
	template <typename F, typename... Args>
	std::function<void()> TFunc(F&& f, Args&&... args) {
		return std::move(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
	}
	template <typename T>
	void Print(T v) {
		//std::cout << sizeof(v) << std::endl;
	}
	template <typename T1, typename T2>
	void Print2(T1 v, T2 v2) {

		//std::cout << sizeof(v) << "," << sizeof(v2) << std::endl;
	}
	struct TestDataConstruct
	{
	};
	TEST(bcore_misc, function) {
		TFunc(Print<int>, 1)();
		TFunc(Print2<int, int>, 2, 22)();
		TFunc(Print<TestDataConstruct>, TestDataConstruct());
	}
	TEST(bcore_misc, btime) {
		int64_t offset = 10;
		auto cur_time = BTime::GetTime();
		BTime::SetOffset(offset);
		EXPECT_EQ(cur_time + offset, BTime::GetTime()) << "check btime offset";
		BTime::SetOffset(0);
		EXPECT_EQ("1970-01-01-08-00-00", BTime::GetTimeFormat(0)) << "check time format";
		//cout << BTime::GetTime() << "," << BTime::GetTimeFormat(0) << endl;
	}
	void GetCount(int&& count) {
		count++;
	}
	void run(std::function<void()> f) {
		f();
	}
	TEST(bcore_misc, timer) {
		auto cur_time = BTime::GetMilliTime();
		int64_t offset = 10;
		int count = 0;
		Timer::AddRepeatedTimer(offset + 3, 5, [cur_time, offset, &count]() {
			count++;
			});
		Timer::AddFuncAfterDuration(offset, [cur_time, offset]() {
			EXPECT_LE(BTime::GetMilliTime() - cur_time - offset, 10) << "check timer duration";
			});

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		EXPECT_EQ(count, 5);
	}

	struct PriorData {
		int value = 0;
		PriorData() {}
		PriorData(int v) : value(v) {}
	};
	struct PriorCmp {
		bool operator() (const PriorData& d1, const PriorData& d2) {
			return d1.value > d2.value;
		}
	};
	TEST(bcore_misc, other) {
		//std::priority_queue<PriorData, vector<PriorData>, PriorCmp> data;
		//for (int i = 0; i < 10; i++) {
		//	data.push(std::rand() % 20);
		//}
		//while (!data.empty()) {
		//	cout << data.top().value << " ";
		//	data.pop();
		//}
		cout << endl;
		{
			auto pool = ObjectPool<PriorData>::NewPool(100, [](PriorData* data) { data->value++; });
			for (int i = 0; i < 10; i++) {
				{
					pool->GetUniqueObject(); 
				}
				{
					pool->GetSharedObject();
				}
			}
			//{
			//	cout << "times:" << pool.GetUniqueObject()->value << endl;
			//}
			{
				cout << "xxxxx\n";
				cout << "times:" << pool->GetSharedObject()->value << endl;
			}
		}
	}

	std::string ToString(std::array<int, 5>& arr) {
		std::stringstream ss;
		for (size_t i = 0; i < arr.size(); i++) {
			ss << arr[i] << " ";
		}
		ss << std::endl;
		return ss.str();
	}
	//TEST(bcore_misc, stat_balance) {
	//	StatBalance info(5);
	//	std::array<int, 5> arr({ 0 });
	//	std::array<int, 5> weight({ 0 });
	//	for (int i = 0; i < 10000; i++) {
	//		auto w = rand() % 20;
	//		auto index = info.AllocIndex(w);
	//		arr[index]++;
	//		weight[index] += w;
	//	}
	//	std::cout << ToString(arr) << "," << ToString(weight) << std::endl;
	//	for (int i = 0; i < 5; i++) {
	//		auto w = rand() % weight[i];
	//		weight[i] -= w;
	//		info.FreeIndex(i, w);
	//	}
	//	std::cout << ToString(arr) << "," << ToString(weight) << std::endl;
	//	for (int i = 0; i < 3000; i++) {
	//		auto w = rand() % 20;
	//		auto index = info.AllocIndex(w);
	//		arr[index]++;

	//		weight[index] += w;
	//	}
	//	std::cout << std::endl;
	//	std::cout << ToString(arr) << "," << ToString(weight) << std::endl;
	//}
}