#pragma once

namespace bcore {
	template <typename T>
	class ThreadLocal {
	public:
		static std::shared_ptr<T> GetInstance() {
			static thread_local std::shared_ptr<T> instance = std::make_shared<T>();
			return instance;
		}
		
	private:
		static void Deleter(T* t) {
			
		}
	};
}