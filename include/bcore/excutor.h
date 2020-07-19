#pragma once
#include <queue>
#include <memory>
#include <mutex>
#include "data_queue.h"

namespace bcore {
	typedef std::shared_ptr<IEvent> EventType;
	class IEvent {
	public:
		virtual ~IEvent() {}
		void Process(){}
	};
	class IExcutor {
	public:
		virtual ~IExcutor() {}
		void Receive(EventType event) {

		}
		void Run() {}
	};

	class Excutor : public IExcutor {
	public:
		void Receive(EventType event) {
			data_queue_.Push(std::move(event));
		}
		void Run() {
			while (data_queue_.Pop(consume_queue_)) {
				while (!consume_queue_.empty()) {
					auto value = std::move(consume_queue_.front());
					consume_queue_.pop();
					value->Process();
				}
			}
		}
	private:
		std::queue<EventType> consume_queue_;
		DataQueue<EventType> data_queue_;
	};
}
