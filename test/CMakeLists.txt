set(test_names test_subprocess test_cat test_env test_err_redirection test_split test_main test_ret_code)
set(test_files env_script.sh write_err.sh write_err.txt)

foreach(test_name IN LISTS test_names)
    add_executable(${test_name} ${test_name}.cc)

    add_test(
        NAME ${test_name}
        COMMAND $<TARGET_FILE:${test_name}>
    )    
endforeach()

foreach(test_file IN LISTS test_files)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/${test_file}
        ${CMAKE_CURRENT_BINARY_DIR}/${test_file}
        COPYONLY
    )
endforeach()
