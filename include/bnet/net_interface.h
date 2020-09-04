#pragma once
#include "bcore/buffer_pool.h"
#include "error_code.h"
#include <functional>

namespace bnet {
	class ISession {
	public:
		virtual ~ISession() {};
		
	};
	class IMessage {
	public:
		virtual ~IMessage() {}
		void ProtocolSize(IMessage* message, int& totalSize) {}
	};

	using ReceiveMessageFunc = std::function<void(void*)>;
	class IMessageHandler {
	public:
		virtual ~IMessageHandler() {}
		virtual uint32_t DecodeMessage(ISession* ses, bcore::Slice slice, ErrorCode& err);
		virtual std::string DecodePartMessage(ISession* ses, bcore::Slice slice, ErrorCode& err);
		virtual void EncodeMessage(ISession* ses, void* message, ErrorCode& err);
		virtual void SetReceiveMessageFunc(ReceiveMessageFunc func) = 0;
		virtual void Init() {}
	};
	class MessageData {

	};
	struct EventPack {

	};

	class IProtoCoder {
	public:
		virtual uint32_t ProtocolSize(void* message, int& msg_type, ErrorCode& err) = 0;
		virtual void ProtocolEncode(void* message, bcore::Slice& buf, int msg_type, ErrorCode& err) = 0;
		virtual void* ProtocolDecode(bcore::Slice& buf, ErrorCode& err, int& msg_type, bool is_part) = 0;
		virtual void ReceiveMessage(void* message) = 0;
	protected:
		ReceiveMessageFunc receive_func_;
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
		virtual bool TryDecodeFrame(bcore::Slice& slice, ErrorCode& err, uint32_t& read_len, bcore::Slice& message_data, bool is_part) = 0;
		virtual void EncodeFrame(uint32_t msg_len, bcore::Slice& buf, ErrorCode& err) = 0;
		virtual uint32_t EncodeFrameLenSize(uint32_t msg_len) = 0;
	};
}