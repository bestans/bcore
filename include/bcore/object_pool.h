#pragma once
#include <vector>
#include <algorithm>
#include <limits>
#include "timer.h"
#include <memory>

namespace bcore {
	const int64_t OBJECT_POOL_CHECK_INTERVAL = 100;
	template <typename T>
	class ObjectPool : public NonCopyable, public NonMovable, public std::enable_shared_from_this<ObjectPool<T>> {
	public:
		typedef std::function<void(T*)> DeleterType;
		typedef std::function<void(T*)> ResetFuncType;

		ObjectPool() : timer_check_interval_(OBJECT_POOL_CHECK_INTERVAL) {
			deleter_ = [this](T* t) { Deleter(t); };
		}
		ObjectPool(int64_t pool_check_interval) : timer_check_interval_(pool_check_interval) {
			deleter_ = [this](T* t) { Deleter(t); };
		}
		ObjectPool(int64_t pool_check_interval, ResetFuncType reset_func) :
			timer_check_interval_(pool_check_interval),
			reset_func_(reset_func)
		{
			deleter_ = [this](T* t) { Deleter(t); };
		}
		~ObjectPool() {
			for (int i = 0; i < count_; i++) {
				delete pool_list_[i];
				pool_list_[i] = nullptr;
			}
			count_ = 0;
		}
		static std::shared_ptr<ObjectPool> NewPool() {
			return std::make_shared<ObjectPool>();
		}
		static std::shared_ptr<ObjectPool> NewPool(int64_t check_interval) {
			return std::make_shared<ObjectPool>(std::move(check_interval));
		}
		static std::shared_ptr<ObjectPool> NewPool(int64_t check_interval, ResetFuncType reset_func) {
			return std::make_shared<ObjectPool>(std::move(check_interval), std::move(reset_func));
		}
		template <typename... FuncArgs>
		std::shared_ptr<T> GetSharedObject(FuncArgs... args) {
			auto object = AllocObject(std::forward<FuncArgs>(args)...);
			if (reset_func_ != nullptr) {
				reset_func_(object);
			}
			return std::shared_ptr<T>(object, deleter_);
		}
		template <typename... FuncArgs>
		std::unique_ptr<T, DeleterType> GetUniqueObject(FuncArgs... args) {
			auto object = AllocObject(std::forward<FuncArgs>(args)...);
			if (reset_func_ != nullptr) {
				reset_func_(object);
			}
			return std::move(std::unique_ptr<T, DeleterType>(object, deleter_));
		}
		template <typename... FuncArgs>
		static std::unique_ptr<T, DeleterType> GetUniqueObjectWithDefault(FuncArgs... args) {
			auto object = new T(std::forward<FuncArgs>(args)...);
			return std::move(std::unique_ptr<T, DeleterType>(object, DefaultDeleter));
		}
	protected:
		void Deleter(T* t) {
			AddObject(t);
		}
		static void DefaultDeleter(T* t) {
			delete t;
		}
		void AddObject(T* t) {
			mutex_.lock();
			if (pool_list_.size() > count_) {
				pool_list_[count_] = t;
			}
			else {
				pool_list_.push_back(t);
			}
			count_++;
			if (timer_node_ == nullptr) {
				//设置定时器回收
				auto self = shared_from_this();
				timer_node_ = Timer::AddFuncAfterDuration(timer_check_interval_, [self]() {
					self->UpdateObject();
					});
			}
			mutex_.unlock();
		}

		template <typename... FuncArgs>
		T* AllocObject(FuncArgs... args) {
			if (count_ <= 0) {
				return new T(std::forward<FuncArgs>(args)...);
			}
			mutex_.lock();
			if (count_ <= 0) {
				mutex_.unlock();
				return new T(std::forward<FuncArgs>(args)...);
			}
			auto object = pool_list_[--count_];
			if (min_count_ > count_) {
				min_count_ = count_;
			}
			mutex_.unlock();
			return object;
		}
		void UpdateObject() {
			mutex_.lock();
			auto remove_count = std::min((min_count_ + 1) / 2, count_);
			std::cout << "UpdateObject:remove_count=" << remove_count << ",min=" << min_count_ << std::endl;
			if (remove_count > 0) {
				if (remove_count >= 64) {
					remove_count >>= 1;
				}
				while (remove_count-- > 0) {
					delete pool_list_[--count_];
				}
			}
			min_count_ = count_;
			if (count_ > 0) {
				auto self = shared_from_this();
				timer_node_ = Timer::AddFuncAfterDuration(timer_check_interval_, [self]() {
					self->UpdateObject();
					});
			}
			else {
				timer_node_.reset();
			}
			mutex_.unlock();
		}
	private:
		DeleterType deleter_;
		std::vector<T*> pool_list_;
		int count_ = 0;
		std::mutex mutex_;
		ResetFuncType reset_func_ = nullptr;
		int min_count_ = 0;
		int timer_check_interval_ = OBJECT_POOL_CHECK_INTERVAL;
		std::shared_ptr<Timer::TimerNode> timer_node_;
	};
}
