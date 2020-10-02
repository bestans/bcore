#include <iostream>
#include <streambuf>
#include <gtest/gtest.h>
#include "bcore/stream/buffer_stream.h"

using namespace bcore;
TEST(buffer_stream, test) {
	std::stringbuf buf;
	IBufferStream ibs(&buf);
	OBufferStream obs(&buf);
	float v = 10;
	obs << v;
	float value = 0;
	ibs >> value;
	std::cout << value << std::endl;
}