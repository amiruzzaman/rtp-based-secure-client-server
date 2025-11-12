add_library(ld_libs INTERFACE)
find_package(Libevent REQUIRED)
target_link_libraries(ld_libs INTERFACE
    libevent::core
    libevent::extra
    libevent::pthreads)

add_library(rtp)
target_sources(rtp
    PRIVATE
    rtp.c
    rtp_util.c
    rtp_session.c
    PUBLIC
    rtp.h
    rtp_util.h
    rtp_err.h
    rtp_session.h
)
target_link_libraries(rtp
    PUBLIC
    compile_opts
    m
    ld_libs
)
target_include_directories(rtp
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_DIR}/include>
)

target_sources(server
    PRIVATE
    server.c
    log.h
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
    log.h
)
target_link_libraries(client
    PRIVATE
    compile_opts
    # math library
    rtp
    m
    ld_libs
)
