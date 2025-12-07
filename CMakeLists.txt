# 根目录 CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# 定义项目名称
project(PartialRead)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 导出 compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 单配置生成器（如 Ninja、Unix Makefiles）
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")


set(ZSTD_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libpr/libzstd.a")

# 添加子目录
add_subdirectory(experiments)

