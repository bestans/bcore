cmake_minimum_required (VERSION 3.8)

file(GLOB SRC_LIST "*.cpp")

add_library(bcore STATIC ${SRC_LIST})
# 将源代码添加到此项目的可执行文件。
# add_executable (bcore ${SRC_LIST})
#message("${SRC_LIST}")
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GOOGLE_TEST_INCLUDE}
)
message(${CMAKE_CURRENT_SOURCE_DIR})
# target_link_libraries(bcore  ${G_TEST_LIB})
