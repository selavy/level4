cmake_minimum_required(VERSION 2.8.11)
project(level4 C CXX)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(BOOST_ROOT $ENV{BOOST_DIR})
find_package(Boost 1.55.0 REQUIRED COMPONENTS program_options)
include_directories(${Boost_INCLUDE_DIRS})

add_compile_options("--std=c++11")
add_compile_options("-Wall")
add_compile_options("-Werror")

add_subdirectory(src)