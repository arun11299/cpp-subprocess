cmake_minimum_required(VERSION 3.5)
project(subprocess VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11 CACHE STRING "C++ standard to use")
option(EXPORT_COMPILE_COMMANDS "create clang compile database" ON)
option(SUBPROCESS_TESTS "enable subprocess tests" OFF)
option(SUBPROCESS_INSTALL "enable subprocess install" OFF)

find_package(Threads REQUIRED)

add_library(subprocess INTERFACE)
target_link_libraries(subprocess INTERFACE Threads::Threads)
target_sources(subprocess PUBLIC
    FILE_SET HEADERS
    FILES
        subprocess/subprocess.hpp
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/
)

if(SUBPROCESS_INSTALL)
    install(FILES subprocess.hpp DESTINATION include/cpp-subprocess/)
endif()

if(SUBPROCESS_TESTS)
    include(CTest)
    add_subdirectory(test)
endif()
