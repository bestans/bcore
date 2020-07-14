cmake_minimum_required (VERSION 3.8)

file(GLOB SRC_LIST "*.cpp")

# 将源代码添加到此项目的可执行文件。
add_executable (bnet ${SRC_LIST})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)