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
		std::shared_ptr<ITimer> timer;
	};
	typedef std::shared_ptr<TimerNode> TimerNodeType;
	class TimerManager : Singleton<TimerManager> {
	public:
		void AddTimer(int64_t delta, std::shared_ptr<ITimer> timer) {
			
		}
		void OnTime(int64_t check_time) {
			std::shared_ptr<ITimer> timer;
			timer->OnFinish();
		}

	protected:
		TimerManager() : thread_(std::bind(&TimerManager::Run, this)) {
		
		}
		void Run() {
			
		}
		void push(TimerNodeType timer) {
			timer_list_.Push(std::move(timer));
		}
	private:
		std::thread thread_;
		DataHeap<TimerNodeType> timer_list_;
	};
}
