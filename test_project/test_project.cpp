// test_project.cpp: 定义应用程序的入口点。
//

#include "test_project.h"
#include "test_buffer.h"
#include "gtest/gtest.h"

using namespace std;
int add(int a, int b) {
	return a + b;
}
TEST(testCase, test0) {
	EXPECT_EQ(add(2, 3), 5);
}
int main(int argc, char** argv) {
	cout << "Hello CMake." << endl;
	print();
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
