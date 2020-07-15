#include <iostream>
#include "buffer_pool.h"
#include "gtest/gtest.h"
using namespace std;

int add(int a, int b) {
	return a + b;
}
TEST(testCase, test0) {
	EXPECT_EQ(add(2, 3), 5);
}

TEST(testCase, Log2Int) {
	bool has_left = false;
	EXPECT_EQ(Log2Int(2, has_left), 0) << "xxs";
}

class TestBuffer {
public:
	TestBuffer(uint32_t size) {
		size_ = size;
	}
	uint32_t size_;
	uint32_t GetSize() const {
		return size_;
	}
};

TEST(testCase, bufferpool) {
	BufferPool<TestBuffer> pool(10, 2);
	auto data = pool.GetBuffer(10);
	cout << "111:" << data->GetSize() << endl;
}

int main(int argc, char** argv) {
	cout << "Hello CMake." << endl;
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
