cmake_minimum_required(VERSION 3.22)
project(network-test
    VERSION 0.0.1
    DESCRIPTION "Network test"
    LANGUAGES C CXX
)

include(CMakeDependentOption)
cmake_dependent_option(NWT_LINK_ASAN
    "Whether to link with AddressSanitizer"
    ON
    "CMAKE_BUILD_TYPE STREQUAL Debug"
    OFF
)
cmake_dependent_option(NWT_LINK_UBSAN
    "Whether to link with UndefinedBehaviorSanitizer"
    ON
    "CMAKE_BUILD_TYPE STREQUAL Debug"
    OFF
)

add_library(compile_opts INTERFACE)
if(NWT_LINK_ASAN)
    target_compile_options(compile_opts INTERFACE "-fsanitize=address")
    target_link_libraries(compile_opts INTERFACE "-fsanitize=address")
endif()
if(NWT_LINK_UBSAN)
    target_compile_options(compile_opts INTERFACE "-fsanitize=undefined")
    target_link_libraries(compile_opts INTERFACE "-fsanitize=undefined")
endif()
if(CMAKE_BUILD_TYPE STREQUAL Debug)
    # fuck it, we only support clang and gcc (and apple clang i guess); no MSVC
    target_compile_options(compile_opts INTERFACE "-Wall;-Wextra;-Wconversion;-Wnull-dereference;-Wformat=2")
endif()

add_executable(server)
add_subdirectory(src)
