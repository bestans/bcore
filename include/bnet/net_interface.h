#pragma once
#include "bcore/buffer_pool.h"

namespace bnet {
	class ISession {
	private:
		virtual ~ISession() = 0;
		
	};
	class IMessage {
	public:
		virtual ~IMessage(){}
		void ProtocolSize(IMessage* message, int& totalSize){}
	}
	
	class IMessageHandler {

	};
	class MessageData {

	};
	struct EventPack {

	};

	class IProtoCoder {
	public:
		virtual void* ProtocolSize(IMessage* message, int& totalSize) {}
		virtual MessageData* ProtocolEncode(IMessage* message, buf[]byte, void* param);
		virtual IMessage* ProtocolDecode(MessageData* data);
	};
	struct ProtoCoderParam {
		int msgType = 0;
	};
	struct DataBuffer {

	};
	typedef std::shared_ptr<DataBuffer> DataBufferType;
	class IFrameProcess {
	public:
		virtual ~IFrameProcess() {}
		virtual bool TryDecodeFrame(bcore::Slice& slice, ErrorCode& err, uint32_t& read_len, bcore::Slice& message_data) = 0;
	};
}