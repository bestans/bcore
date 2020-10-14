#include <iostream>
#include <streambuf>
#include <gtest/gtest.h>
#include "bcore/stream/buffer_stream.h"
#include <gtest/gtest.h>

using namespace bcore;
TEST(buffer_stream, test) {
	FixedStringBuf buf(1000);
	IOBufferStream iobs(&buf);
	bool v1 = true;
	char v2 = 100;
	unsigned char v3 = 101;
	int16_t v4 = INT16_MAX;
	uint16_t v5 = UINT16_MAX;
	int32_t v6 = INT32_MAX;
	uint32_t v7 = UINT32_MAX;
	int64_t  v8 = INT64_MAX;
	uint64_t v9 = UINT64_MAX;
	const char* v10 = "v10";
	std::string v11 = "string10";
	iobs << v1 << v2 << v3 << v4 << v5 << v6 << v7 << v8 << v9 << v10;
	auto d1 = v1;
	auto d2 = v2;
	auto d3 = v3;
	auto d4 = v4;
	auto d5 = v5;
	auto d6 = v6;
	auto d7 = v7;
	auto d8 = v8;
	auto d9 = v9;
	auto d10 = v11;
	d1 = 0; d2 = 0; d3 = 0; d4 = 0; d5 = 0; d6 = 0; d7 = 0; d8 = 0; d9 = 0;
	d10.clear();
	iobs >> d1 >> d2 >> d3 >> d4 >> d5 >> d6 >> d7 >> d8 >> d9 >> d10;

	EXPECT_EQ(v1, d1);
	EXPECT_EQ(v2, d2);
	EXPECT_EQ(v3, d3);
	EXPECT_EQ(v4, d4);
	EXPECT_EQ(v5, d5);
	EXPECT_EQ(v6, d6);
	EXPECT_EQ(v7, d7);
	EXPECT_EQ(v8, d8);
	EXPECT_EQ(v9, d9);
	EXPECT_EQ(v10, d10);
	iobs << v11;
	d10.clear();
	iobs >> d10;
	EXPECT_EQ(v11, d10);
	std::cout << d1 << "," << (int)d2 << "," << (int)d3 << "," << d4 << "," << d5 << "," << d6 << "," << d7 << "," << d8 << "," << d9 << "," << d10 << std::endl;
}

TEST(buffer_stream, testinvalid) {
	StringBuf buf;
	IOBufferStream iobs(&buf);
	iobs << (int)100;
	buf.SeekBegin();
	iobs << (int)110;
	int v = 0;
	iobs >> v;
	std::cout << v << std::endl;

	FixedStringBuf fbuf(5);
	std::iostream ss(&fbuf);
	std::cout << ss.good() << std::endl;
	ss << "aaaa" << "aasdfa" << 11;
	ss.clear();
	std::cout << ss.good() << std::endl;
	fbuf.SeekBegin();
	ss << "aaaa" << "aasdfa" << 11;
	std::cout << fbuf.str() << std::endl;
}