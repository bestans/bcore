#include <iostream>
#include "gtest/gtest.h"
#include "test_asio.h"

using namespace std;

int main222(int argc, char** argv) {
	cout << "Hello CMake." << endl;
	test_timer();
	testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();
	cout << "finish\n";
	system("PAUSE");
	return 0;
}
