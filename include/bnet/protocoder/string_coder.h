#pragma once
#include "bnet/net_interface.h"

namespace bnet {
	class StringCoder : public IProtoCoder {
	public:
		virtual void* ProtocolSize(IMessage* message, int& totalSize) override
		{
			
		}


		virtual MessageData* ProtocolEncode(IMessage* message, buf[]byte, void* param) override
		{
			throw std::logic_error("The method or operation is not implemented.");
		}


		virtual IMessage* ProtocolDecode(MessageData* data) override
		{
			throw std::logic_error("The method or operation is not implemented.");
		}
	};
}