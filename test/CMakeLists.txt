cmake_minimum_required (VERSION 3.8)

file(GLOB SRC_LIST "*.cpp")

# 将源代码添加到此项目的可执行文件。
add_executable (bcore_test ${SRC_LIST})
# message("111${CMAKE_CURRENT_SOURCE_DIR}")
include_directories(
    # F:/github_code/googletest-release-1.10.0/googletest/include
)

target_link_libraries(bcore_test ${G_TEST_LIB})

add_subdirectory ("test_asio")