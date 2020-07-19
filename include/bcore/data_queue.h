#pragma once
#include <memory>
#include <mutex>
#include <deque>

namespace bcore {
	enum EM_DATA_QUEUE_POP_RESULT {
		EM_DATA_QUEUE_INVALID = 0,		////停止并且没数据了
		EM_DATA_QUEUE_SUCCESS,
		EM_DATA_QUEUE_VALID_BUT_STOP,	//已经停止了但是还有数据
	};
	template <class T>
	class DataQueue {
		static const T defaultT;
	public:
		bool Push(T t) {
			std::unique_lock<std::mutex> lock(mutex_);
			if (stop_) {
				return false;
			}
			data_queue_.push_back(std::move(t));
			condition_.notify_one();
			return true;
		}
		void Stop() {
			std::unique_lock<std::mutex> lock(mutex_);
			if (stop_) {
				return;
			}
			stop_ = true;
			condition_.notify_all();
		}
		EM_DATA_QUEUE_POP_RESULT Pop(std::deque<T>& receive) {
			std::unique_lock<std::mutex> lock(mutex_);
			while (data_queue_.empty() && !stop_) {
				condition_.wait(lock);
			}
			auto has_data = !data_queue_.empty();

			receive.clear();
			std::swap(receive, data_queue_);
			if (!stop_ || has_data) {
				return EM_DATA_QUEUE_SUCCESS;
			}
			else if (has_data) {
				return EM_DATA_QUEUE_VALID_BUT_STOP;
			}
			else {
				return EM_DATA_QUEUE_INVALID;
			}
		}
		EM_DATA_QUEUE_POP_RESULT Pop(T& value) {
			std::unique_lock<std::mutex> lock(mutex_);
			while (data_queue_.empty() && !stop_) {
				condition_.wait(lock);
			}
			auto has_data = !data_queue_.empty();
			if (has_data) {
				std::swap(value, data_queue_.front());
				data_queue_.pop_front();
			}
			if (!stop_ || has_data) {
				return EM_DATA_QUEUE_SUCCESS;
			}
			else if (has_data) {
				return EM_DATA_QUEUE_VALID_BUT_STOP;
			}
			else {
				return EM_DATA_QUEUE_INVALID;
			}
		}

	private:
		std::mutex mutex_;
		std::condition_variable condition_;
		std::deque<T> data_queue_;
		bool stop_ = false;
	};
}