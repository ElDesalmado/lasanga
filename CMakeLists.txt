cmake_minimum_required(VERSION 3.7.2 FATAL_ERROR)

project(lasanga LANGUAGES CXX)

# TODO: 14/11 and customizable?
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# TODO: options for clean build and testing

add_library(lasanga INTERFACE)
target_include_directories(lasanga INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(sandbox sandbox.cpp)
target_link_libraries(sandbox lasanga)

add_executable(sb.builder sb_builder.cpp)
target_link_libraries(sb.builder lasanga)

# TODO: condition
add_subdirectory(examples)