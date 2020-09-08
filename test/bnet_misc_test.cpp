#include <gtest/gtest.h>
#include "bnet/share_coder.h"
#include <system_error>
#include "bnet/error_code.h"

using namespace bnet;
using namespace bcore;

TEST(bnet, share_coder) {
	auto buf = BufferPool::AllocSharedBuffer(10);
	auto slice = Slice(buf->data(), buf->cap(), 0);
	for (int i = 1; i < 1000; i++) {
		slice.reset_len(0);
		ShareCoder::EncodeVarint(slice, i);
		uint32_t read_len;
		uint64_t x;
		ShareCoder::DecodeVarint(slice, read_len, x);
		EXPECT_EQ(i, x);
		EXPECT_EQ(read_len, ShareCoder::EncodeVarintSize(i));
	}
}
TEST(bnet, share_coder2) {
	bnet::ErrorCode e((bnet::ERROR_CODE)1);
	bnet::ErrorCode e2((bnet::ERROR_CODE)1, "asdfasdfasfd");
	std::cout << e.message() << "," << !e << "," << (e == e2) << (e != e2) << "," << e2.message() << std::endl;
}