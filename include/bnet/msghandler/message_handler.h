#pragma once
#include "bcore/buffer_pool.h"
#include "bnet/net_interface.h"
#include <memory>
#include <functional>
#include "bnet/msghandler/frame_process.h"
#include "bnet/protocoder/string_coder.h"

namespace bnet {
	
	class MessageHandler : public IMessageHandler {
	public:
		MessageHandler() :
			frame_(LengthFrameBuilder::NewInstance()),
			proto_coder_(new StringCoder())
		{
		}
		static std::shared_ptr<MessageHandler> Instance() {
			static std::shared_ptr<MessageHandler> g_instance = std::make_shared<MessageHandler>();
			return g_instance;
		}
		uint32_t DecodeMessage(ISession* ses, bcore::Slice slice, ErrorCode& err) {
			uint32_t read_len = 0;
			bcore::Slice message_data;
			if (!frame_->TryDecodeFrame(slice, err, read_len, message_data, false)) {
				return 0;
			}
			if (err) {
				return 0;
			}
			if (!message_data) {
				err = ERROR_CODE::kDecodeMessageFailed;
				return 0;
			}
			int msg_type = 0;
			auto msg = proto_coder_->ProtocolDecode(message_data, err, msg_type, false);
			receive_func_(ses, msg);
			return read_len;
		}
		std::string DecodePartMessage(ISession* ses, bcore::Slice slice, ErrorCode& err) override {
			uint32_t read_len = 0;
			bcore::Slice message_data;
			if (!frame_->TryDecodeFrame(slice, err, read_len, message_data, true)) {
				return 0;
			}
			int msg_type = 0;
			proto_coder_->ProtocolDecode(message_data, err, msg_type, true);
			std::string str;
			str += "err:";
			str += err.message();
			str += ",msg_type";
			str += msg_type;
			str += ",read_len=";
			str += read_len;
			return std::move(str);
		}
		bcore::UniqueByteBuf EncodeMessage(ISession* ses, void* message, ErrorCode& err) {
			int msg_type = 0;
			uint32_t msg_size = proto_coder_->ProtocolSize(message, msg_type, err);
			if (err) {
				return bcore::UniqueByteBuf();
			}
			auto total_size = msg_size + frame_->EncodeFrameLenSize(msg_size);
			auto buffer = bcore::BufferPool::AllocBuffer(total_size);
			bcore::Slice slice = buffer->to_slice();
			frame_->EncodeFrame(msg_size, slice, err);
			if (err) {
				return bcore::UniqueByteBuf();
			}
			proto_coder_->ProtocolEncode(message, slice, msg_type, err);
			if (err) {
				return bcore::UniqueByteBuf();
			}
			buffer->add_len(slice.len());
			return std::move(buffer);
		}
		bool IsValidProtoType(type_info& other) {
			return proto_coder_->IsValidProtoType(other);
		}
		void SetReceiveMessageFunc(ReceiveMessageFunc func) override {
			proto_coder_->SetMessageFunc(std::move(func));
			receive_func_ = func;
		}
	private:
		std::shared_ptr<IFrameProcess> frame_;
		std::shared_ptr<IProtoCoder> proto_coder_;
		ReceiveMessageFunc receive_func_;
	};
}