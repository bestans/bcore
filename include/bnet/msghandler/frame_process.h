#pragma once
#include "bnet/share_coder.h"

namespace bnet {
	class LengthFrameBuilder : public IFrameProcess {
	public:
		static std::shared_ptr<LengthFrameBuilder> NewInstance() {
			static std::shared_ptr<LengthFrameBuilder> g_instance = std::make_shared<LengthFrameBuilder>();
			return g_instance;
		}
		LengthFrameBuilder() {

		}
		bool TryDecodeFrame(bcore::Slice& slice, ErrorCode& err, uint32_t& read_len, bcore::Slice& message_data, bool is_part) override {
			uint32_t len_len = 0;
			uint64_t msg_len = 0;
			if (!ShareCoder::DecodeVarint(slice, len_len, msg_len)) {
				err = ERROR_CODE::kDecodeVarint;
				return false;
			}
			read_len = msg_len + len_len;
			if (slice.len() < read_len) {
				if (is_part) {
					message_data = slice.make_slice(len_len, slice.len());
				}
				return false;
			}
			message_data = slice.make_slice(len_len, read_len);
			return true;
		}

		virtual void EncodeFrame(uint32_t msg_len, bcore::Slice& buf, ErrorCode& err) override
		{
			if (!ShareCoder::EncodeVarint(buf, msg_len)) {
				err = ERROR_CODE::kEncodeVarintBufNotEnough;
			}
		}


		virtual uint32_t EncodeFrameLenSize(uint32_t msg_len) override
		{
			return ShareCoder::EncodeVarintSize(msg_len);
		}

	};
}