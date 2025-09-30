cmake_minimum_required(VERSION 3.23)
project(network-test
    VERSION 0.0.1
    DESCRIPTION "Network test"
    LANGUAGES C CXX
)

include(CMakeDependentOption)
include(CheckLinkerFlag)
include(CTest)

check_linker_flag(C -fsanitize=address RTP_MOD_LINKER_HAS_ASAN)
check_linker_flag(C -fsanitize=undefined RTP_MOD_LINKER_HAS_UBSAN)
cmake_dependent_option(RTP_MOD_LINK_ASAN
    "Whether to link with AddressSanitizer"
    ON
    "CMAKE_BUILD_TYPE STREQUAL Debug;RTP_MOD_LINKER_HAS_ASAN"
    OFF
)
cmake_dependent_option(RTP_MOD_LINK_UBSAN
    "Whether to link with UndefinedBehaviorSanitizer"
    ON
    "CMAKE_BUILD_TYPE STREQUAL Debug;RTP_MOD_LINKER_HAS_UBSAN"
    OFF
)
option(BUILD_TESTING "Whether to build the tests" OFF)

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
    # we only support clang and gcc (and apple clang i guess); no MSVC
    target_compile_options(compile_opts INTERFACE
        "-Wall;-Wextra;-Wconversion;-Wnull-dereference;-Wformat=2;-Wno-missing-braces")
endif()

add_executable(server)
add_executable(client)
add_subdirectory(src)
if(BUILD_TESTING)
    add_subdirectory(test)
endif()
