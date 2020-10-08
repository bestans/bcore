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
	obs << (float)10000;
	obs << (int)1234;
	float fvalue = 0;
	int ivalue = 0;
	ibs >> fvalue >> ivalue;
	std::cout << ivalue << "," << fvalue << std::endl;
}