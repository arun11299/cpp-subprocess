add_executable(test_subprocess test_subprocess.cc)

add_test(
    NAME test_subprocess
    COMMAND $<TARGET_FILE:test_subprocess>
)

add_executable(test_cat test_cat.cc)

add_test(
    NAME test_cat
    COMMAND $<TARGET_FILE:test_cat>
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/env_script.sh ${CMAKE_CURRENT_BINARY_DIR}/env_script.sh COPYONLY)
add_executable(test_env test_env.cc)

add_test(
    NAME test_env
    COMMAND $<TARGET_FILE:test_env>
)

add_executable(test_err_redirection test_err_redirection.cc)

add_test(
    NAME test_err_redirection
    COMMAND $<TARGET_FILE:test_err_redirection>
)

add_executable(test_split test_split.cc)

add_test(
    NAME test_split
    COMMAND $<TARGET_FILE:test_split>
)

add_executable(test_main test.cc)

add_test(
    NAME test_main
    COMMAND $<TARGET_FILE:test_main>
)

