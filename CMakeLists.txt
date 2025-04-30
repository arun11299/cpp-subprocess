cmake_minimum_required(VERSION 3.5)
project(subprocess VERSION 2.2 LANGUAGES CXX)

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
        cpp-subprocess/subprocess.hpp
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/
)
add_library(cpp-subprocess::subprocess ALIAS subprocess)

if(SUBPROCESS_INSTALL)
    install(
        TARGETS subprocess COMPONENT subprocess
        EXPORT subprocess
        FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    include(CMakePackageConfigHelpers)

    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/subprocess-config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/subprocess-config.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/subprocess"
        PATH_VARS PROJECT_NAME PROJECT_VERSION
    )

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/beman.exemplar-version.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY ExactVersion
    )

    install(
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/subprocess-config.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/subprocess-version.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/subprocess"
        COMPONENT subprocess
    )

    install(
        EXPORT subprocess
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/beman.exemplar"
        NAMESPACE cpp-subprocess::
        FILE subprocess-targets.cmake
        COMPONENT subprocess
    )
endif()

if(SUBPROCESS_TESTS)
    include(CTest)
    add_subdirectory(test)
endif()
