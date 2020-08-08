#pragma once
#include "basio/tcp_session.h"
#include "bnet/net_interface.h"

namespace bnet {
	class IMessageHandler {
	public:
		virtual ~IMessageHandler() = 0;
		
		void EncodeMessage(ISession* ses, IMessage* msg) {}
		int DecodeMessage(ISession* ses, MessageData* data) {}
		void HandleEvent(ISession* ses, EventPack* event){}
	};
	class MessageHandler : public IMessageHandler {
	public:
		virtual ~MessageHandler() {}

		void EncodeMessage(ISession* ses, IMessage* msg) override {
			
		}
	private:
		IProtoCoder* proto_coder_;
	};
}
