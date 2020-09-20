#pragma once
#include <memory>
#include <mutex>
#include <vector>
#include <algorithm>
#include <array>
#include <queue>
#include "singleton.h"
#include "btime.h"
#include "data_struct/data_heap.h"

namespace bcore {
	void TestCoreLib();
	static int timer_node_times = 0;
	class Timer : Singleton<Timer> {
	private:
		typedef std::function<void()> TimerFunc;
	public:
		struct TimerNode {
			friend Timer;
			TimerNode(int64_t end_t, TimerFunc& f) : end_time(end_t), stop(false), func(std::move(f)) {
				timer_node_times++;
			}
			TimerNode(int64_t end_t, TimerFunc&& f) : end_time(end_t), stop(false), func(std::move(f)) {
				timer_node_times++;
			}
			void StopTimer() { stop = true; }
			~TimerNode() {
				timer_node_times--;
			}
		protected:
			int64_t end_time;
			bool stop;
			TimerFunc func;
		};
		typedef std::shared_ptr<TimerNode> TimerNodeType;

		template <typename F, typename... Args>
		inline static TimerNodeType AddFuncAfterDuration(int64_t duration, F&& f, Args&&... args) {
			return Timer::Instance().addTimer(duration + BTime::GetMilliTime(), std::move(std::bind(std::forward<F>(f), std::forward<Args>(args)...)));
		}
		template <typename F, typename... Args>
		static TimerNodeType AddRepeatedTimer(int64_t duration, int repeat_num, F&& f, Args&&... args) {
			return AddRepeatedTimerLocal(duration, repeat_num, std::move(std::bind(std::forward<F>(f), std::forward<Args>(args)...)));
		}
		static void Stop() {
			Timer::Instance().running_ = false;
			Timer::Instance().condition_.notify_all();
		}
		Timer() : thread_([this]() {Run(); }) { }
	protected:
		static TimerNodeType AddRepeatedTimerLocal(int64_t duration, int repeat_num, TimerFunc f) {
			return Timer::Instance().addTimer(duration + BTime::GetMilliTime(), [local_f = std::move(f), repeat_num, duration]{
				local_f();
				if (repeat_num > 1 || repeat_num == -1) {
					AddRepeatedTimerLocal(duration, repeat_num == -1 ? -1 : repeat_num - 1, std::move(local_f));
				}
				});
		}
		void Run() {
			while (running_) {
				{
					std::unique_lock<std::mutex> lock(mutex_);
					int64_t check_time = BTime::GetMilliTime();
					while (!timer_list_.empty() && timer_list_.top()->stop) {
						timer_list_.pop();
					}
					while ((timer_list_.empty()) && running_) {
						condition_.wait(lock);
						continue;
					}
					if (!running_) {
						return;
					}
					auto left_time = timer_list_.top()->end_time - check_time;
					if (left_time > 0) {
						condition_.wait_for(lock, std::chrono::milliseconds(left_time));
						continue;
					}
					while (!timer_list_.empty() && timer_list_.top()->end_time <= check_time && wait_count_ < MaxWaitProcessTimerCount) {
						wait_list_[wait_count_++] = std::move(timer_list_.top());
						timer_list_.pop();
					}
				}
				while (wait_count_ > 0) {
					auto timer_node = std::move(wait_list_[--wait_count_]);
					if (!timer_node->stop) {
						timer_node->func();
					}
				}
			}
		}
		TimerNodeType addTimer(int64_t end_time, TimerFunc&& f) {
			std::unique_lock<std::mutex> lock(mutex_);
			auto is_empty = timer_list_.empty();
			auto timerNode = std::make_shared<TimerNode>(end_time, std::move(f));
			timer_list_.push(timerNode);
			if (is_empty) {
				condition_.notify_one();
			}
			return timerNode;
		}

		struct TimerNodeCmp {
			bool operator() (const TimerNodeType& d1, const TimerNodeType& d2) {
				return d1->end_time > d2->end_time;
			}
		};
	private:
		static const std::size_t MaxWaitProcessTimerCount = 20;

		std::array<TimerNodeType, MaxWaitProcessTimerCount> wait_list_;
		int wait_count_ = 0;
		std::mutex mutex_;
		std::condition_variable condition_;
		std::thread thread_;
		std::priority_queue<TimerNodeType, std::vector<TimerNodeType>, TimerNodeCmp> timer_list_;
		bool running_ = true;
	};
}
