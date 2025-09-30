# if CMake cannot find libevent by itself, use pkg-config
# I'm pretty sure the package is only found if you use conan
add_library(ld_libs INTERFACE)
find_package(Libevent QUIET)
if(NOT Libevent_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBEVENT REQUIRED libevent)
    target_link_libraries(ld_libs INTERFACE ${LIBEVENT_LIBRARIES})
else()
    find_package(Libevent REQUIRED)
    target_link_libraries(ld_libs INTERFACE libevent::libevent)
endif()

add_library(rtp)
target_sources(rtp
    PRIVATE
    rtp.c
    rtp_util.c
    PUBLIC
    rtp.h
    rtp_util.h
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
