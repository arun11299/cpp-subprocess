cmake_minimum_required(VERSION 3.1)
project(subprocess VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11 CACHE STRING "C++ standard to use")
option(EXPORT_COMPILE_COMMANDS "create clang compile database" ON)
option(SUBPROCESS_TESTS "enalbe subprocess tests" OFF)

find_package(Threads REQUIRED)

add_library(subprocess INTERFACE)
target_link_libraries(subprocess INTERFACE Threads::Threads)
target_include_directories(subprocess INTERFACE . )
install(FILES subprocess.hpp DESTINATION include/cpp-subprocess/)

if(SUBPROCESS_TESTS)
    include(CTest)
    add_subdirectory(test)
endif()
