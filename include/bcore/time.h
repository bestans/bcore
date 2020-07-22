#pragma once
#include "singleton.h"
#include <chrono>
#include <ctime>
namespace bcore {
	class BTime : public Singleton<BTime> {
	public:
		static int64_t GetTime() {
			return std::chrono::duration_cast<std::chrono::seconds>(GetTimePoint()).count();
		}
		static int64_t GetMilliTime() {
			return std::chrono::duration_cast<std::chrono::milliseconds>(GetTimePoint()).count();
		}
		static void SetOffset(int64_t offset) {
			auto seconds = std::chrono::seconds(offset);
			BTime::Instance().offset = std::chrono::duration_cast<std::chrono::system_clock::duration>(seconds).count();
		}
		static std::time_t GetTimeT(int64_t time) {
			auto duration = std::chrono::duration_cast<std::chrono::system_clock::duration>(
				std::chrono::seconds(time));
			return std::chrono::system_clock::to_time_t(std::chrono::system_clock::time_point(duration));
		}
	private:
		static std::chrono::system_clock::duration GetTimePoint() {
			auto time_point = std::chrono::system_clock::now().time_since_epoch().count();
				+ BTime::Instance().offset;
			return std::chrono::system_clock::duration(time_point);
		}
	private:
		int64_t offset = 0;
	};
}