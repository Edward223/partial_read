# 根目录 CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# 定义项目名称
project(PartialRead)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 导出 compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 添加子目录
add_subdirectory(exam_utils)

set(ZSTD_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libpr/libzstd.a")
