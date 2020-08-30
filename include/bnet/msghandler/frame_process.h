#pragma once
#include "bnet/share_coder.h"

namespace bnet {
	class LengthFrameBuilder : public IFrameProcess {
	public:
		LengthFrameBuilder() {

		}
		bool TryDecodeFrame(bcore::Slice& slice, ErrorCode& err, uint32_t& read_len, bcore::Slice& message_data) override {
			uint32_t len_len = 0;
			uint64_t msg_len = 0;
			if !ShareCoder::DecodeVarint(slice, len_len, msg_len) {
				err = ERROR_CODE.kDecodeVarint;
				return false;
			}
			auto read_len = msg_len + len_len;
			if (slice.len() < read_len) {
				return false;
			}
			return true;
		}
	};
}