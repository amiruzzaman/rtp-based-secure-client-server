# .rst:
# FindLibevent
# -------------
#
# Find libevent.
#
# Imported Targets
# ^^^^^^^^^^^^^^^^
#
# ``libevent::libevent``
# ``libevent::core``
# ``libevent::extra``
# ``libevent::openssl``
# ``libevent::pthreads`` (if not on Windows)

# mostly copy from CMake's shipped FindSQLite3 module
# TODO: do we support static linking?

function(__find_libevent_create_import_target _target _import_loc _incl_dir)
    if(NOT TARGET ${_target})
        add_library(${_target} UNKNOWN IMPORTED)
        set_target_properties(${_target} PROPERTIES
            IMPORTED_LOCATION ${${_import_loc}}
            INTERFACE_INCLUDE_DIRECTORIES ${${_incl_dir}})
    endif()
endfunction()

macro(__find_libevent_find_component_library _component_var _component_name _dir_hint)
    find_library(${_component_var}
             NAMES
             ${_component_name}
             PATH_SUFFIXES lib
             HINTS
             ${_dir_hint})
    mark_as_advanced(${_component_var})
endmacro()

find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(PC_Libevent QUIET libevent)
    pkg_check_modules(PC_Libevent_core QUIET libevent_core)
    pkg_check_modules(PC_Libevent_extra QUIET libevent_extra)
    pkg_check_modules(PC_Libevent_openssl QUIET libevent_openssl)
    pkg_check_modules(PC_Libevent_pthreads QUIET libevent_pthreads)
endif()

find_path(Libevent_INCLUDE_DIR
         NAMES
         event2/event.h
         PATH_SUFFIXES include
         HINTS
         ${PC_Libevent_INCLUDE_DIRS})
mark_as_advanced(Libevent_INCLUDE_DIR)
message("${Libevent_INCLUDE_DIR}")

__find_libevent_find_component_library(Libevent_LIBRARY event PC_Libevent_LIBRARY_DIRS)
__find_libevent_find_component_library(Libevent_core_LIBRARY event_core PC_Libevent_LIBRARY_DIRS)
__find_libevent_find_component_library(Libevent_extra_LIBRARY event_extra PC_Libevent_LIBRARY_DIRS)
__find_libevent_find_component_library(Libevent_openssl_LIBRARY event_openssl PC_Libevent_LIBRARY_DIRS)
if(NOT WIN32)
    __find_libevent_find_component_library(Libevent_pthreads_LIBRARY event_pthreads PC_Libevent_LIBRARY_DIRS)
endif()

# get version
if(Libevent_INCLUDE_DIR)
    if(PC_Libevent_VERSION)
        # the easy way
        set(Libevent_VERSION ${PC_Libevent_VERSION})
    else()
        # and the hard way
        file(STRINGS ${Libevent_INCLUDE_DIR}/event2/event-config.h __ver_line
             REGEX "^#define EVENT__VERSION *\"[0-9]+\\.[0-9]+\\.[0-9]+.*\""
             LIMIT_COUNT 1)
        string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+"
               Libevent_VERSION "${__ver_line}")
        unset(__ver_line)
    endif()
endif()

include(FindPackageHandleStandardArgs)
# NOTE: the libevent book says that one day, the libevent one might be nuked
# but the "component" libraries (core, extra, openssl, pthread) should still be
# around
# also openssl and pthreads are not always present (aka, platform dependent)
find_package_handle_standard_args(Libevent
    REQUIRED_VARS
    Libevent_LIBRARY
    Libevent_core_LIBRARY
    Libevent_extra_LIBRARY
    Libevent_INCLUDE_DIR
    VERSION_VAR Libevent_VERSION)

# import target
if(Libevent_FOUND)
    set(Libevent_INCLUDE_DIRS ${Libevent_INCLUDE_DIR})
    __find_libevent_create_import_target(libevent::libevent Libevent_LIBRARY Libevent_INCLUDE_DIR)
    __find_libevent_create_import_target(libevent::core Libevent_core_LIBRARY Libevent_INCLUDE_DIR)
    __find_libevent_create_import_target(libevent::extra Libevent_extra_LIBRARY Libevent_INCLUDE_DIR)
    __find_libevent_create_import_target(libevent::openssl Libevent_openssl_LIBRARY Libevent_INCLUDE_DIR)
    if(NOT WIN32)
        __find_libevent_create_import_target(libevent::pthreads Libevent_pthreads_LIBRARY Libevent_INCLUDE_DIR)
    endif()
endif()
