#include <gtest/gtest.h>
#include "bnet/share_coder.h"

using namespace bnet;
using namespace bcore;

TEST(bnet, share_coder) {
	auto slice = std::make_shared<Slice>(10);
	for (int i = 1; i < 1000; i++) {
		slice->reset_len(0);
		ShareCoder::EncodeVarint(slice, i);
		uint32_t read_len;
		uint64_t x;
		ShareCoder::DecodeVarint(slice, read_len, x);
		EXPECT_EQ(i, x);
		EXPECT_EQ(read_len, ShareCoder::EncodeVarintSize(i));
	}
}