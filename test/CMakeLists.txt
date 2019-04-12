

add_executable(test_subprocess test_subprocess.cc)

add_test(
    NAME test_subprocess
    COMMAND $<TARGET_FILE:test_subprocess>
)