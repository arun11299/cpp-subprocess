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

add_executable(test_env test_env.cc)

add_test(
    NAME test_env
    COMMAND $<TARGET_FILE:test_env>
)

