#pragma once
#include "net_interface.h"
#include <memory>

namespace bnet {
	class MessageHandler : public IMessageHandler {
	public:
		uint32_t DecodeMessage(ISession* ses, bcore::Slice slice, ErrorCode& err) {
			uint32_t read_len = 0;
			bcore::Slice message_data(nullptr, 0);
			if !(frame_->TryDecodeFrame(ses, err, read_len, message_data)) {
				return 0;
			}
			if (!err) {
				return 0;
			}
		}
	private:
		std::shared_ptr<IFrameProcess> frame_;
	};
}