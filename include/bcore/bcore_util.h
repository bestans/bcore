#pragma once 
#include <chrono>

namespace bcore {
	template <class T, class N>
	T* SafeStaticCast(N* ptr) {
		if (ptr == nullptr) {
			return nullptr;
		}
		if (typeid(*ptr) == typeid(T)) {
			return static_cast<*T>(ptr);
		}
		return nullptr;
	}
	class StopWatch {
	public:
		StopWatch() {
			begin_time_ = std::chrono::high_resolution_clock::now();
		}
		int64_t CostSeconds() {
			auto endTime = std::chrono::high_resolution_clock::now();
			auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(endTime - begin_time_);
			return elapsedTime.count();
		}
		int64_t CostMilliSeconds() {
			auto endTime = std::chrono::high_resolution_clock::now();
			auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - begin_time_);
			return elapsedTime.count();
		}
		int64_t CostMicroSeconds() {
			auto endTime = std::chrono::high_resolution_clock::now();
			auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - begin_time_);
			return elapsedTime.count();
		}
	private:
		std::chrono::nanoseconds Cost() {
			return std::chrono::high_resolution_clock::now() - begin_time_;
		}
	private:
		std::chrono::high_resolution_clock::time_point begin_time_;
	};
}
