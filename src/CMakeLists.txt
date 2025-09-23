# if CMake cannot find libevent by itself, use pkg-config
find_package(Libevent)
if(NOT Libevent_FOUND)
    find_package(PkgConfig)
    pkg_check_modules(LIBEVENT REQUIRED libevent)
else()
    set(LIBEVENT_LIBRARIES Libevent)
endif()

target_sources(server
    PRIVATE
    server.c
)
target_link_libraries(server
    PRIVATE
    compile_opts
    # math library
    m
    ${LIBEVENT_LIBRARIES}
)
