# TODO: build tests

find_package(Libevent REQUIRED)

function(add_test_executable target)
    add_executable(${target})
    target_sources(${target}
        PRIVATE
        ${target}.c
    )
    target_link_libraries(${target}
        PRIVATE
        compile_opts
        rtp
        libevent::core
        libevent::extra
    )
    string(TOUPPER ${target} target_upper)
    add_test(NAME ${target_upper}
        COMMAND ${target}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endfunction()

add_test_executable(test_header_serialize)
