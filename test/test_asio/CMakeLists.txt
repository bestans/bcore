cmake_minimum_required (VERSION 3.8)

file(GLOB TEST_ASIO_SRC_LIST "*.cpp")

include_directories(
    F:/github_code/asio-1.16.1/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
# 将源代码添加到此项目的可执行文件。
add_executable (test_asio ${TEST_ASIO_SRC_LIST})

target_link_libraries(test_asio ${G_TEST_LIB} ${BCORE_LIB})
