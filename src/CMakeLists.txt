target_sources(server
    PRIVATE
    server.c
    log.h
)
target_link_libraries(server
    PRIVATE
    compile_opts
    # math library
    m
)

target_sources(client
    PRIVATE
    client.c
    log.h
)
target_link_libraries(client
    PRIVATE
    compile_opts
    # math library
    m
)
