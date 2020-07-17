# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

project ("bcore-project")

# 包含子项目。
add_subdirectory ("test_project")
add_subdirectory ("network")
add_subdirectory ("src/bcore")
add_subdirectory ("test")


include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
