#pragma once
#include <vector>
#include <algorithm>

namespace bcore {
	template <typename T, typename... Args>
	class ObjectPool {
	public:
		ObjectPool() : reset_func_([](T*, Args...) { }) {

		}
		ObjectPool(std::function<void(T*, Args...)> reset_func) : reset_func_(reset_func) {

		}
		std::shared_ptr<T> GetSharedObject(Args... args) {
			auto object = AllocObject();
			reset_func_(object, std::forward<Args>(args)...);
			return std::shared_ptr<T>(object, Deleter);
		}
		std::unique_ptr<T> GetUniqueObject(Args... args) {
			auto object = AllocObject();
			reset_func_(object, std::forward<Args>(args)...);
			return std::unique_ptr<T>(object, Deleter);
		}
	protected:
		static void Deleter(T* t) {
			Instance().AddObject(t);
		}
		void AddObject(T* t) {
			pool_list_.push_back(t);
			count++;
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
			mutex_.unlock();
			return object;
		}
	private:
		std::vector<T*> pool_list_;
		int count = 0;
		std::mutex mutex_;
		std::function<void(T*, Args...)> reset_func_;
	};
}
