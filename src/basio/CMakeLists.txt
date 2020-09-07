cmake_minimum_required (VERSION 3.8)

file(GLOB ASIO_SRC_LIST "*.cpp")

# 将源代码添加到此项目的可执行文件。
add_library(bcore_asio STATIC ${ASIO_SRC_LIST} "tcp_server.cpp")
#message("${SRC_LIST}")
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ROOT_INCLUDE}/basio
    F:/github_code/asio-1.16.1/include
)
message(${ROOT_INCLUDE}/basio)
message(${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(bcore_asio  ${BCORE_LIB})
