# CMakeList.txt: test_project 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

file(GLOB SRC_LIST "*.cpp" "test_buffer/*.cpp")

# 将源代码添加到此项目的可执行文件。
add_executable (test_project ${SRC_LIST})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/test_buffer
    F:/github_code/googletest-release-1.10.0/googletest/include
)

link_directories(
    F://github_code//googletest-release-1.10.0-cmake//lib//Release
)
find_library(gtest-lib gtest)
target_link_libraries(test_project F://github_code//googletest-release-1.10.0-cmake//lib//Debug//gtestd.lib)
# TODO: 如有需要，请添加测试并安装目标。

