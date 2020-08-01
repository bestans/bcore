# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

project ("bcore-project")

IF (CMAKE_BUILD_TYPE MATCHES "Debug")
    SET(LIB_DIR "lib/x64-release/")
ELSEIF(CMAKE_BUILD_TYPE MATCHES "Release")
    SET(LIB_DIR "lib/x64-debug/")
ELSE()
    SET(CMAKE_BUILD_TYPE "")
ENDIF()

link_directories(
    F://github_code//googletest-release-1.10.0-cmake//lib//Release
)
SET(G_TEST_LIB ${LIB_DIR}gtest)
message(${G_TEST_LIB} ${CMAKE_BUILD_TYPE})

# 包含子项目。
add_subdirectory ("test_project")
add_subdirectory ("network")
add_subdirectory ("src/bcore")
add_subdirectory ("test")


include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
