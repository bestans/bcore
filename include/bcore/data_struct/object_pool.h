#pragma once
#include <vector>
#include <algorithm>
#include <limits>

namespace bcore {
	template <typename T, typename... Args>
	class ObjectPool {
		typedef std::function<void(T*)> DeleterType;
	public:
		ObjectPool() : reset_func_([](T*, Args...) {}) {
			deleter_ = [this](T* t) { Deleter(t); };
		}
		ObjectPool(std::function<void(T*, Args...)> reset_func) : reset_func_(reset_func) {
			deleter_ = [this](T* t) { Deleter(t); };
		}
		~ObjectPool() {
			for (int i = 0; i < count; i++) {
				delete pool_list_[i];
				pool_list_[i] = nullptr;
			}
			count = 0;
		}
		std::shared_ptr<T> GetSharedObject(Args... args) {
			auto object = AllocObject();
			reset_func_(object, std::forward<Args>(args)...);
			return std::shared_ptr<T>(object, deleter_);
		}
		std::unique_ptr<T, DeleterType> GetUniqueObject(Args... args) {
			auto object = AllocObject();
			reset_func_(object, std::forward<Args>(args)...);
			return std::move(std::unique_ptr<T, DeleterType>(object, deleter_));
		}
	protected:
		void Deleter(T* t) {
			AddObject(t);
		}
		void AddObject(T* t) {
			mutex_.lock();
			pool_list_.push_back(t);
			count++;
			if (max_count_ < count) {
				max_count_ = count;
			}
			mutex_.unlock();
			cout << "addobject:" << count << endl;
		}
		
		T* AllocObject() {
			if (count <= 0) {
				return new T();
			}
			mutex_.lock();
			if (count <= 0) {
				mutex_.unlock();
				return new T();
			}
			auto object = pool_list_[--count];
			if (min_count_ > count) {
				min_count_ = count;
			}
			mutex_.unlock();
			return object;
		}
		void UpdateObject() {
			mutex_.lock();
			auto left_count = (min_count_ == std::numeric_limits<int>::max() || max_count_ == 0) ? count / 2 : max_count_ - min_count_;
			if (left_count > 0) {
				if (left_count >= 64) {
					left_count >>= 1;
				}
				while (count > left_count) {
					delete pool_list_[--count];
				}
			}
			min_count_ = std::numeric_limits<int>::max();
			max_count_ = 0;
			mutex_.unlock();
		}
	private:
		std::vector<T*> pool_list_;
		int count = 0;
		std::mutex mutex_;
		std::function<void(T*, Args...)> reset_func_;
		DeleterType deleter_;
		int min_count_ = std::numeric_limits<int>::max();
		int max_count_ = 0;
	};
}
