cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(doctest GIT_REPOSITORY https://github.com/doctest/doctest.git GIT_TAG v2.4.12)
FetchContent_MakeAvailable(doctest)

project(general_tree)

file(GLOB_RECURSE TEST_SOURCES "test/*.cpp")
add_executable(general_tree_tests ${TEST_SOURCES})
target_include_directories(
        general_tree_tests
        PRIVATE
        ${doctest_SOURCE_DIR}/doctest
        ${CMAKE_CURRENT_SOURCE_DIR}  
        ${CMAKE_CURRENT_SOURCE_DIR}/test        
)

enable_testing()
include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(general_tree_tests)