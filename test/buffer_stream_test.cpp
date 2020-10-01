#include <iostream>
#include <streambuf>
#include <gtest/gtest.h>

TEST(buffer_stream, test) {
	std::stringbuf buf;
	std::istream ibs(&buf);
	std::ostream obs(&buf);
	obs << 100;
	int value = 0;
	ibs >> value;
	std::cout << value << std::endl;
}