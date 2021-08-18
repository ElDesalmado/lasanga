cmake_minimum_required(VERSION 3.7.2 FATAL_ERROR)

project(lasanga LANGUAGES CXX)

get_directory_property(HAS_PARENT PARENT_DIRECTORY)

if (NOT HAS_PARENT)
    option(ALL_WARNINGS "Maximum level for warnings" ON)
    option(CLEAN_BUILD "Treat all warnings as errors" ON)
    option(BUILD_EXAMPLES "Build examples" ON)

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)
endif ()

if (ALL_WARNINGS)
    message(STATUS "lasanga: Maximum warning level")

    if (MSVC)
        add_compile_options(/W4)
    else()
        add_compile_options(-Wall -Wextra -pedantic)
    endif()
endif()

if (CLEAN_BUILD)
    message(STATUS "lasanga: Treating all warnings as errors")

    if (MSVC)
        add_compile_options(/WX)
    else()
        add_compile_options(-Werror)
    endif()
endif()

# TODO: 14/11 and customizable?
# TODO: if included as a subdirectory, assert that user's standard is valid
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_library(lasanga INTERFACE)
target_include_directories(lasanga INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(eld::lasanga ALIAS lasanga)

get_directory_property(IS_EXCLUDED EXCLUDE_FROM_ALL)

# examples
if (NOT IS_EXCLUDED AND BUILD_EXAMPLES)
    message(STATUS "lasanga: Building examples")
    # sandbox
    add_subdirectory(examples)

    add_executable(sandbox sandbox.cpp)
    target_link_libraries(sandbox lasanga)

    add_executable(sb.builder sb_builder.cpp)
    target_link_libraries(sb.builder lasanga)

    add_executable(sb.iteration sb_iteration.cpp)
    target_link_libraries(sb.iteration lasanga)
endif()