# TODO: build tests

macro(add_test_executable target)
    add_executable(${target})
    target_sources(${target}
        PRIVATE
        ${target}.c
    )
    target_link_libraries(${target}
        PRIVATE
        compile_opts
        rtp
    )
endmacro()

add_test_executable(test_header_serialize)
