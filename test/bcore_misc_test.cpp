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
			std::shared_ptr<ITimer> ptr1;
			{
				std::shared_ptr<ITimer> vv = std::make_shared<Timer1>();
				ptr1 = vv;
			}
			void* ptra = new Timer1();
			cout << (static_cast<TestSingleton*>(ptra) == nullptr) << endl;
			//cout << (dynamic_cast<TestSingleton*>(ptra) == nullptr) << endl;
			delete ptra;
		}
	}
}