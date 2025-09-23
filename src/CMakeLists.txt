# if CMake cannot find libevent by itself, use pkg-config
# I'm pretty sure the package is only found if you use conan
add_library(ld_libs INTERFACE)
find_package(Libevent)
if(NOT Libevent_FOUND)
    find_package(PkgConfig)
    pkg_check_modules(LIBEVENT REQUIRED libevent)
    target_link_libraries(ld_libs INTERFACE ${LIBEVENT_LIBRARIES})
else()
    find_package(Libevent REQUIRED)
    target_link_libraries(ld_libs INTERFACE libevent::libevent)
endif()

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
    m
    ld_libs
)
