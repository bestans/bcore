#pragma once
#include "bcore/buffer_pool.h"
#include "error_code.h"

namespace bnet {
	class ISession {
	private:
		virtual ~ISession() = 0;
		
	};
	class IMessage {
	public:
		virtual ~IMessage() {}
		void ProtocolSize(IMessage* message, int& totalSize) {}
	};
	
	class IMessageHandler {
	public:
		virtual ~IMessageHandler() {}
		virtual uint32_t DecodeMessage(ISession* ses, bcore::Slice slice, ErrorCode& err);
		virtual void EncodeMessage(ISession* ses, void* message, ErrorCode& err);
	};
	class MessageData {

	};
	struct EventPack {

	};

	class IProtoCoder {
	public:
		virtual uint32_t ProtocolSize(IMessage* message, int& msg_type, ErrorCode& err) {}
		virtual void ProtocolEncode(IMessage* message, bcore::Slice& buf, int msg_type, ErrorCode& err);
		virtual std::shared_ptr<IMessage> ProtocolDecode(bcore::Slice& buf, ErrorCode& err, int& msg_type);
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
		virtual void EncodeFrame(uint32_t msg_len, bcore::Slice& buf, ErrorCode& err) = 0;
		virtual uint32_t EncodeFrameLenSize(uint32_t msg_len) = 0;
	};
}