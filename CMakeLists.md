```bash
#指定 cmake 的最小版本
cmake_minimum_required (VERSION 3.8)

#设置项目名称
project ("bcore")

#包含子项目。
#引入两个变量 demo_BINARY_DIR 和 demo_SOURCE_DIR
#cmake 自动定义了两个等价的变量 PROJECT_BINARY_DIR 和 PROJECT_SOURCE_DIR
add_subdirectory ("test_project")

#设置编译类型
#add_library 默认生成是静态库，通过以上命令生成文件名字：
#在linux下：demo libcommon.a libcommon.so
#在windows下：demo.exe common.lib common.dll
add_executable(demo demo.cpp) # 生成可执行文件
add_library(common STATIC util.cpp) # 生成静态库
add_library(common SHARED util.cpp) # 生成动态库或共享库

#明确指定包含哪些文件
add_library(demo demo.cpp test.cpp util.cpp)

#搜索所有的 cpp 文件
aux_source_directory(. SRC_LIST) # 搜索当前目录下的所有.cpp文件
add_library(demo ${SRC_LIST})

#自定义搜索规则
file(GLOB SRC_LIST "*.cpp" "protocol/*.cpp")
add_library(demo ${SRC_LIST})
# 或者
file(GLOB SRC_LIST "*.cpp")
file(GLOB SRC_PROTOCOL_LIST "protocol/*.cpp")
add_library(demo ${SRC_LIST} ${SRC_PROTOCOL_LIST})
# 或者
aux_source_directory(. SRC_LIST)
aux_source_directory(protocol SRC_PROTOCOL_LIST)
add_library(demo ${SRC_LIST} ${SRC_PROTOCOL_LIST})
```
