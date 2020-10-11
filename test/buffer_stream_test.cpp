#include <iostream>
#include <streambuf>
#include <gtest/gtest.h>
#include "bcore/stream/buffer_stream.h"

using namespace bcore;
TEST(buffer_stream, test) {
	std::stringbuf buf;
	IBufferStream ibs(&buf);
	OBufferStream obs(&buf);
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
	obs << v1 << v2 << v3 << v4 << v5 << v6 << v7 << v8 << v9 << v10;
	v1 = 0; v2 = 0; v3 = 0; v4 = 0; v5 = 0; v6 = 0; v7 = 0; v8 = 0; v9 = 0;
	std::string d10;
	ibs >> v1 >> v2 >> v3 >> v4 >> v5 >> v6 >> v7 >> v8 >> v9 >> d10;
	std::cout << v1 << "," << (int)v2 << "," << (int)v3 << "," << v4 << "," << v5 << "," << v6 << "," << v7 << "," << v8 << "," << v9 << "," << d10 << std::endl;
}
