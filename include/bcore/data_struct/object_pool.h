#pragma once
#include <vector>
#include <algorithm>

namespace bcore {
	template <typename T>
	class ObjectPool {
	public:
		static ObjectPool& Instance() {
			static ObjectPool instance;
			return instance;
		}
		template <typename... Args>
		std::shared_ptr<T> GetObject(Args... args) {
			return std::shared_ptr<T>(new T(std::forward<Args>(args)...), Deleter);
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
	private:
		std::vector<T*> pool_list_;
		int count = 0;

	};
}