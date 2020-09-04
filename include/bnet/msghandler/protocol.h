#pragma once
#include "bnet/net_interface.h"
namespace bnet {
	class StringProtocol : public IMessage {
	public:
		StringProtocol(bcore::Slice& slice) : str_(slice.data(), slice.len()) {}
	private:
		std::string str_;
	};
}