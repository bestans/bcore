#pragma once
#include "bnet/net_interface.h"

namespace bnet {
	class StringCoder : public IProtoCoder {
	public:
		StringCoder() : IProtoCoder(std::bind(&StringCoder::ReceiveMessage, this)) {}
		virtual void* ProtocolSize(void* message, int& totalSize) override
		{
			
		}

		void ProtocolEncode(void* message, bcore::Slice& buf, int msg_type, ErrorCode& err) override
		{
			return
		}

		void* ProtocolDecode(bcore::Slice& buf, ErrorCode& err, int& msg_type, bool is_part) override
		{
			if (is_part) {
				return nullptr;
			}
			return new std::string(buf.data(), buf.len());
		}
		void ReceiveMessage(void* message) {
			auto str = static_cast<*std::string>(message);
			delete str;
		}
	};
}