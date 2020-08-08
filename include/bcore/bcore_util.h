#pragma once 

namespace bcore {
	template <class T, class N>
	T* SafeStaticCast(N* ptr) {
		if (ptr == nullptr) {
			return nullptr;
		}
		if (typeid(*ptr) == typeid(T)) {
			return static_cast<*T>(ptr);
		}
		return nullptr;
	}
}
