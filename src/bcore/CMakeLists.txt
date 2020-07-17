cmake_minimum_required (VERSION 3.8)

file(GLOB SRC_LIST "*.cpp")

# 将源代码添加到此项目的可执行文件。
add_executable (bcore ${SRC_LIST})
#message("${SRC_LIST}")
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    F:/github_code/googletest-release-1.10.0/googletest/include
)

target_link_libraries(bcore F://github_code//googletest-release-1.10.0-cmake//lib//Debug//gtestd.lib)
