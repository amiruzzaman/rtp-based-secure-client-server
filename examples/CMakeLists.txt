add_executable(server)
add_executable(client)

target_sources(server
    PRIVATE
    server.c
)
target_link_libraries(server
    PRIVATE
    compile_opts
    # math library
    rtp
    m
    ld_libs
)

target_sources(client
    PRIVATE
    client.c
)
target_link_libraries(client
    PRIVATE
    compile_opts
    # math library
    rtp
    m
    ld_libs
)
