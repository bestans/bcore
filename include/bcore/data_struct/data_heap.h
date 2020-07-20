#pragma once
#include <memory>
#include <mutex>
#include <vector>
#include <algorithm>
#include <functional>

namespace bcore {
	template <class T>
	class DataHeap {
	public:
		typedef std::function<bool(const T& t1, const T& t2)> DataCompareFunc;
		//Ä¬ÈÏ×îÐ¡¶Ñ
		DataHeap() : compare_func_(std::greater<T>()) {}
		DataHeap(DataCompareFunc func) : compare_func_(std::move(func)) {}
		void Push(T data) {
			data_list_.emplace_back(std::move(data));
			std::push_heap(data_list_.begin(), data_list_.end(), compare_func_);
		}
		bool Pop(T& data) {
			if (data_list_.empty()) {
				return false;
			}
			std::swap(data, data_list_[0]);
			std::pop_heap(data_list_.begin(), data_list_.end(), compare_func_);
			data_list_.pop_back();
			return true;
		}
		T& Top() {
			return data_list_[0];
		}
	private:
		std::vector<T> data_list_;
		DataCompareFunc compare_func_;
	};

}