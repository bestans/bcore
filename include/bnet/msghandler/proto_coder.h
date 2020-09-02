#pragma once

namespace bnet {
	class StringCoder : public IProtoCoder {

	public:
		virtual uint32_t ProtocolSize(void* message, int& msg_type, ErrorCode& err) override
		{
			return static_cast<*std::string>(message)->size();
		}
		virtual void ProtocolEncode(void* message, bcore::Slice& buf, int msg_type, ErrorCode& err) override
		{
			auto str = static_cast<*std::string>(message);
			if (!buf.append(str->c_str(), str->size())) {
				err = ERROR_CODE.kProtocolEncodeBufNotEnough;
				return;
			}
		}
		virtual std::shared_ptr<IMessage> ProtocolDecode(bcore::Slice& buf, ErrorCode& err, int& msg_type) override
		{
			return std::make_shared<std::string>(buf.data(), buf.len());
		}
	};
}