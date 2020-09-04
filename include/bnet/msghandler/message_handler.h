#pragma once
#include "bcore/buffer_pool.h"
#include "bnet/net_interface.h"
#include <memory>
#include <functional>
#include <sstring>

namespace bnet {
	using ReceiveMessageFunc = std::function<void(void*)>;
	
	class MessageHandler : public IMessageHandler {
	public:
		uint32_t DecodeMessage(ISession* ses, bcore::Slice slice, ErrorCode& err) {
			uint32_t read_len = 0;
			bcore::Slice message_data;
			if (!frame_->TryDecodeFrame(slice, err, read_len, message_data, false)) {
				return 0;
			}
			if (!err) {
				return 0;
			}
			if (!message_data) {
				err = ERROR_CODE::kDecodeMessageFailed;
				return 0;
			}
			int msg_type = 0;
			auto msg = proto_coder_->ProtocolDecode(message_data, err, msg_type);
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
		void EncodeMessage(ISession* ses, void* message, ErrorCode& err) {
			int msg_type = 0;
			uint32_t msg_size = proto_coder_->ProtocolSize(message, msg_type, err);
			if (!err) {
				return;
			}
			auto total_size = msg_size + frame_->EncodeFrameLenSize(msg_size);
			auto buffer = bcore::BufferPool::AllocBuffer(total_size);
			bcore::Slice slice = buffer->to_slice();
			frame_->EncodeFrame(msg_size, slice, err);
			if (!err) {
				return;
			}
			proto_coder_->ProtocolEncode(message, slice, msg_type, err);
			if (!err) {
				return;
			}
			buffer->add_len(slice.len());
		}
	private:
		std::shared_ptr<IProtoCoder> proto_coder_;
		std::shared_ptr<IFrameProcess> frame_;
		ReceiveMessageFunc receive_func_;
	};
}