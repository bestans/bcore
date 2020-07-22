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
	template <typename F, typename... Args>
	std::function<void()> TFunc(F&& f, Args&&... args) {
		return std::move(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
	}

	template <typename T>
	void Print(T v) {
		std::cout << sizeof(v) << std::endl;
	}
	template <typename T1, typename T2>
	void Print2(T1 v, T2 v2) {

		std::cout << sizeof(v) << "," << sizeof(v2) << std::endl;
	}
	struct TestDataConstruct
	{
		int value = 10;
		TestDataConstruct() {
			cout << "TestDataConstruct\n";
		}
		TestDataConstruct(const TestDataConstruct& other) {
			cout << "TestDataConstruct&\n";
		}
		TestDataConstruct(TestDataConstruct&& other) {
			cout << "TestDataConstruct&&\n";
		}
		TestDataConstruct& operator= (const TestDataConstruct& other) {
			cout << "=TestDataConstruct&\n";
		}
		TestDataConstruct& operator= (TestDataConstruct&& other) {
			cout << "=TestDataConstruct&&\n";
		}
	};
	TEST(bcore_misc, function) {
		TFunc(Print<int>, 1)();
		TFunc(Print2<int, int>, 2, 22)();
		TFunc(Print<TestDataConstruct>, TestDataConstruct());
	}
	class NonCopyable
	{
	public:
		NonCopyable() = default;
		NonCopyable(const NonCopyable&) = delete;
		NonCopyable& operator = (const NonCopyable&) = delete;
	};

	/*
	 * 阻止移动基类
	 */
	class NonMovable
	{
	public:
		NonMovable() = default;
		NonMovable(NonMovable&&) = delete;
		NonMovable& operator = (NonMovable&&) = delete;
	};

	/*
	 * 饿汉单例模式基类
	 */
	template <typename T>
	class SingletonNew
	{
	public:
		static T* GetInstance() { return &s_instance; }
	public:
		static T s_instance;
		SingletonNew() {};
	};
	template <typename TT>
	TT SingletonNew<TT>::s_instance;

	class TestSingleton1 : public SingletonNew<TestSingleton1> {
	public:
		int value = 10;
		TestSingleton1() {
			cout << "TestSingleton111111111111111\n";
		}
	};

	TEST(bcore_misc, TestSingleton) {
		cout << "TestSingleton0\n";
		cout << TestSingleton1::GetInstance()->value << endl;
		cout << "TestSingleton2\n";
	}
	TEST(bcore_misc, btime) {
		cout << BTime::GetTime() << endl;
		auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

		auto t2 = BTime::GetTimeT(BTime::GetTime());
		//转为字符串
		std::stringstream ss;
		ss << std::put_time(std::localtime(&t), "%Y-%m-%d-%H-%M-%S");
		ss << std::put_time(std::localtime(&t2), "%Y-%m-%d-%H-%M-%S");
		cout << ss.str() << endl;
	}
}