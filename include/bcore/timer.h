#pragma once
namespace bcore {
	class ITimer {
	public:
		ITimer() {}
		virtual ~ITimer() {};
		virtual void OnFinish() = 0;
	};
	struct TimerNode {
		int64_t end_time = 0;
		bool stop = false;
		std::shared_ptr<ITimer> timer;
	};

	class TimerManager {
	public:
		void AddTimer(int64_t delta, std::shared_ptr<ITimer> timer) {
			
		}
		void OnTime() {
			std::shared_ptr<ITimer> timer;
			timer->OnFinish();
		}
	private:

	};
}
