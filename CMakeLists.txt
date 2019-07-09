cmake_minimum_required(VERSION 3.1)

project(subprocess CXX)

set(CMAKE_CXX_STANDARD 11)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(test)
endif()

install(FILES subprocess.hpp DESTINATION include/subprocess/)
