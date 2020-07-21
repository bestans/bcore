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
		T Pop() {
			if (data_list_.empty()) {
				return T();
			}
			auto value = std::move(data_list_[0]);
			std::pop_heap(data_list_.begin(), data_list_.end(), compare_func_);
			data_list_.pop_back();
			return std::move(value);
		}
		T& Top() {
			return data_list_[0];
		}
		bool Empty() const {
			return data_list_.empty();
		}
		size_t Size() const {
			return data_list_.size();
		}
	private:
		std::vector<T> data_list_;
		DataCompareFunc compare_func_;
	};

}