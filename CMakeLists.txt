cmake_minimum_required(VERSION 3.10.0)
project(MEngineGL VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 设置可执行文件的输出目录 (bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# 设置动态库（共享库）的输出目录 (lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# 设置静态库的输出目录 (lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(Tool)
add_subdirectory(Function)
add_subdirectory(Resource)
add_subdirectory(Core)
add_subdirectory(Platform)
add_subdirectory(Common)


enable_testing()
add_subdirectory(Test)





