cmake_minimum_required(VERSION 3.1)

project(subprocess CXX)

set(CMAKE_CXX_STANDARD 11)

# Options
option(BUILD_TESTING "Build tests" OFF)

# Tests
include(CTest)
if(BUILD_TESTING)
    add_subdirectory(test)
endif()

add_library(subprocess INTERFACE)
target_include_directories(subprocess INTERFACE .)

install(FILES subprocess.hpp DESTINATION include/cpp-subprocess/)
