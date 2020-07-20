#include "bcore.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <atomic>
using namespace std;
using namespace bcore;
namespace test {

	class TestSingleton : public Singleton<TestSingleton> {
	public:
		int value = 0;
		void Init() override {
			value += 100;
		}
	};

	class Timer1 : public ITimer {
	public:
		~Timer1() {
			cout << "~Timer1\n";
		}
	};

	TEST(bcore_misc, singleton) {
		EXPECT_EQ(TestSingleton::Instance().value, 100);
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
}