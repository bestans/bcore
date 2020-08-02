# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

project ("bcore-project")

IF (CMAKE_BUILD_TYPE MATCHES "Debug")
    SET(LIB_DIR "lib/x64-debug")
ELSEIF(CMAKE_BUILD_TYPE MATCHES "Release")
    SET(LIB_DIR "lib/x64-release")
ELSE()
    SET(CMAKE_BUILD_TYPE "")
ENDIF()

link_directories(
    ${LIB_DIR}
)
SET(G_TEST_LIB ${CMAKE_CURRENT_SOURCE_DIR}/${LIB_DIR}/gtest.lib)

message(G_TEST_LIB\ ${G_TEST_LIB})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    F:/github_code/googletest-release-1.10.0/googletest/include
    F:/github_code/asio-1.16.1/include
)

# 包含子项目。
add_subdirectory ("test_project")
add_subdirectory ("network")
add_subdirectory ("src/bcore")
add_subdirectory ("test")

