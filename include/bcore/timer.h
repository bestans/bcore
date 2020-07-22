#pragma once
#include <memory>
#include <mutex>
#include <vector>
#include <algorithm>
#include "singleton.h"
#include "data_struct/data_heap.h"

namespace bcore {
	class ITimer {
	public:
		ITimer() {}
		virtual ~ITimer() {};
		virtual void OnFinish() =0;
	};
	struct TimerNode {
		int64_t end_time = 0;
		bool stop = false;
		std::function<void()> func;
	};
	typedef std::shared_ptr<TimerNode> TimerNodeType;
	class TimerManager : Singleton<TimerManager> {
	public:
		template <typename F, typename... Args>
		void AddFuncAfterDuration(int64_t end_time, F&& f, Args&&... args) {
			TimerNode node;
			node.end_time = end_time;
			node.func = std::move(std::bind(std::forward<F>(f), std::forward<Args>(args)...))
		}
		template <typename F, typename... Args>
		void AddRepeatedTimer(int64_t end_time, int repeat_num, F&& f, Args&&... args) {
			AddRepeatedTimerLocal(end_time, repeat_num, std::move(std::bind(std::forward<F>(f), std::forward<Args>(args)...)));
		}
		void AddRepeatedTimerLocal(int64_t duration, int repeat_num, std::function<void()> f) {
			TimerNode node;
			node.end_time = duration;
			node.func = [this, local_f = std::move(f), repeat_num, duration]{
				local_f();
				if (repeat_num > 1) {
					AddRepeatedTimerLocal(duration, repeat_num - 1, std::move(local_f));
				}
			};
		}
		void AddTimer(int64_t delta, std::shared_ptr<ITimer> timer) {
			
		}
		void OnTime(int64_t check_time) {
			std::shared_ptr<ITimer> timer;
			timer->OnFinish();
		}

	protected:
		TimerManager() : thread_([this](){Run();}) {
		
		}
		void Run() {
			while (true) {
				{
					std::unique_lock<std::mutex> lock(mutex_);
					do {
						if (stop_) {
							return;
						}
						if (timer_list_.Empty()) {
							condition_.wait(lock);
							continue;
						}
						int64_t cur_time = 0;
						auto left_time = timer_list_.Top()->end_time - cur_time;
						if (left_time > 0) {
							condition_.wait_for(lock, std::chrono::milliseconds(left_time));
							continue;
						}
					} while (false);
					int64_t check_time = 0;
					while (!timer_list_.Empty() && timer_list_.Top()->end_time <= check_time && wait_process_list_.size() < MaxWaitProcessTimerCount) {
						wait_process_list_.emplace_back(std::move(timer_list_.Pop()));
					}
				}
				while (!wait_process_list_.empty()) {
					auto timer_node = std::move(wait_process_list_.front());
					if (!timer_node->stop) {
						timer_node->func();
					}
					wait_process_list_.pop_front();
				}
			}
		}
		void push(TimerNodeType timer) {
			std::unique_lock<std::mutex> lock(mutex_);
			timer_list_.Push(std::move(timer));
			condition_.notify_one();
		}
	private:
		const std::size_t MaxWaitProcessTimerCount = 20;

		std::deque<TimerNodeType> wait_process_list_;
		std::mutex mutex_;
		std::condition_variable condition_;
		std::thread thread_;
		DataHeap<TimerNodeType> timer_list_;
		bool stop_ = false;
	};
}
