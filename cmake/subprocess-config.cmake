include(CMakeFindDependencyMacro)
find_dependency(Threads)

set(SUBPROCESS_VERSION @PROJECT_VERSION@)

@PACKAGE_INIT@

include(${CMAKE_CURRENT_LIST_DIR}/@PROJECT_NAME@-targets.cmake)

check_required_components(@PROJECT_NAME@)
