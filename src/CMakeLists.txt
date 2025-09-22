target_sources(server
    PRIVATE
    server.c
)
target_link_libraries(server
    PRIVATE
    compile_opts
    # math library
    m
)
