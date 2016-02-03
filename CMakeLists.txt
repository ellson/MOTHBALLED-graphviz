# A CMake build file for Graphviz.

# TODO:
# - SONAME versioning
# - Add missing libs/plugins
# - Fix config.h.in
#   - Write platform checks -- see config.h.in
#   - Extend options
#   - more...
# - Make shared libraries work
#   - I started, then quickly gave up (cyclic dependencies all over the place)
#   - I'm not sure how this is solved in the autotools build
#     AFAIU it is generating both static libs + shared libs and then links a set of static libs into a shared one
#
# Tested platforms

cmake_minimum_required (VERSION 2.8.11) # feature: string(TIMESTAMP ...)

project(Graphviz C)

include(CheckIncludeFiles)
include(CheckSymbolExists)
include(CheckCCompilerFlag)
include(CheckSymbolExists)
include(CMakeParseArguments)

# BEGIN: Constants
set(GV_VERSION_MAJOR "2")
set(GV_VERSION_MINOR "39")
set(GV_VERSION_PATCH "0")
set(GV_VERSION "${GV_VERSION_MAJOR}.${GV_VERSION_MINOR}.${GV_VERSION_PATCH}")

# TODO: Triggers rebuild of complete project all the time. Fix: Move defines using GV_VERSION_TIMESTAMP to separate file
#string(TIMESTAMP GV_VERSION_TIMESTAMP "%Y%m%d.%H%M")
set(GV_VERSION_TIMESTAMP "0")

set(GV_VERSION_STRING "${GV_VERSION}.${GV_VERSION_TIMESTAMP}")

set(GV_PLUGIN_CONFIG_FILE "config6")
set(GV_PLUGIN_VERSION 6)
# END

# BEGIN: Options
option(ENABLE_LTDL "On-demand plugin loading" OFF)
set(GV_GRAPH_LIB graphviz_lib_cgraph)
# END

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

find_package(EXPAT REQUIRED)

find_library(M_LIBRARY m)
if (NOT M_LIBRARY)
    set(M_LIBRARY "")
endif()
find_library(Z_LIBRARY z)
if (NOT Z_LIBRARY)
    set(Z_LIBRARY "")
endif()

# BEGIN: Platform checks
check_include_files(expat.h       HAVE_EXPAT_H)
check_include_files(limits.h      HAVE_LIMITS_H)
check_include_files(malloc.h      HAVE_MALLOC_H)
check_include_files(memory.h      HAVE_MEMORY_H)
check_include_files(ndir.h        HAVE_NDIR_H)
check_include_files(search.h      HAVE_SEARCH_H)
check_include_files(stdarg.h      HAVE_STDARG_H)
check_include_files(stddef.h      HAVE_STDDEF_H)
check_include_files(stdint.h      HAVE_STDINT_H)
check_include_files(stdlib.h      HAVE_STDLIB_H)
check_include_files(strings.h     HAVE_STRINGS_H)
check_include_files(string.h      HAVE_STRING_H)
check_include_files(sys/fpu.h     HAVE_SYS_FPU_H)
check_include_files(sys/inotify.h HAVE_SYS_INOTIFY_H)
check_include_files(sys/ioctl.h   HAVE_SYS_IOCTL_H)
check_include_files(sys/mman.h    HAVE_SYS_MMAN_H)
check_include_files(unistd.h      HAVE_UNISTD_H)
check_include_files(values.h      HAVE_VALUES_H)
check_include_files(zlib.h        HAVE_ZLIB_H)

check_symbol_exists(log2         "math.h"    HAVE_LOG2)
check_symbol_exists(pow          "math.h"    HAVE_POW)
check_symbol_exists(sincos       "math.h"    HAVE_SINCOS)
check_symbol_exists(sqrt         "math.h"    HAVE_SQRT)
check_symbol_exists(drand48      "stdlib.h"  HAVE_DRAND48)
check_symbol_exists(srand48      "stdlib.h"  HAVE_SRAND48)
check_symbol_exists(setenv       "stdlib.h"  HAVE_SETENV)
check_symbol_exists(strncasecmp  "strings.h" HAVE_STRNCASECMP)
check_symbol_exists(strcasecmp   "strings.h" HAVE_STRCASECMP)
check_symbol_exists(strcasestr   "strings.h" HAVE_STRCASESTR)

# Compat with original configure
# HACK: Disable for now
#set(HAVE_EXPAT ${HAVE_EXPAT_H} AND ${EXPAT_FOUND})
#set(HAVE_LIBZ ${HAVE_ZLIB_H})
if (WIN32)
    set(VAR_PATHSEPARATOR ";")
else()
    set(VAR_PATHSEPARATOR ":")
endif()
# END

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)
#
# Compiler & linker settings
#
add_definitions(-DHAVE_CONFIG_H -DHAVE_SETMODE)
# FIXME: Export macro usage is super duper broken in Graphviz... (completely inconsistent, needs a complete revamp)
# let's just make sure we never attempt to use __declspec(dllimport) in this static build
add_definitions(-DGVC_EXPORTS -DCGRAPH_EXPORTS -D_BLD_cdt)
if (NOT HAVE_UNISTD_H)
    add_definitions("-DYY_NO_UNISTD_H")
endif()
if (MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4242 /wd4244 /wd4305 /wd4996")
    set(CMAKE_C_FLAGS "/MP ${CMAKE_C_FLAGS}")
endif()

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_MODULE_LINKER_FLAGS}")
endif()

set(INCLUDE_INSTALL_DIR "include/graphviz")
set(INSTALL_TARGETS_DEFAULT_ARGS
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib COMPONENT Devel
)

macro(bison_with_replace INPUT YY_REPLACEMENT)
    string(REPLACE ".y" "" OUTPUT_BASENAME ${INPUT})
    bison_target(${OUTPUT_BASENAME}.pre.c ${INPUT} ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_BASENAME}.pre.c)

    add_custom_command(OUTPUT ${OUTPUT_BASENAME}.h
        COMMAND sed "s/yy/${YY_REPLACEMENT}/g" ${OUTPUT_BASENAME}.pre.h > ${OUTPUT_BASENAME}.h
        DEPENDS ${OUTPUT_BASENAME}.pre.h
    )
    add_custom_command(OUTPUT ${OUTPUT_BASENAME}.c
        COMMAND sed "s/yy/${YY_REPLACEMENT}/g" ${OUTPUT_BASENAME}.pre.c > ${OUTPUT_BASENAME}.c
        DEPENDS ${OUTPUT_BASENAME}.pre.c
    )
endmacro()

set(GRAPHVIZ_LIBRARIES)

function(add_gvlib TARGET)
    set(options )
    set(oneValueArgs TYPE)
    set(multiValueArgs)
    cmake_parse_arguments(add_gvlib "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    add_library(${TARGET} ${add_gvlib_TYPE} ${add_gvlib_UNPARSED_ARGUMENTS})
    string(REPLACE "graphviz_lib_" "" OUTPUT_NAME ${TARGET})
    set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME ${OUTPUT_NAME})
    if (NOT DEFINED add_gvlib_TYPE OR add_gvlib_TYPE STREQUAL STATIC)
        set_target_properties(${TARGET} PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()

    install(TARGETS ${TARGET} EXPORT GraphvizTargets ${INSTALL_TARGETS_DEFAULT_ARGS})
endfunction()

function(add_gvplugin TARGET)
    add_library(${TARGET} ${ARGN})
    string(REPLACE "graphviz_plugin_" "" OUTPUT_NAME ${TARGET})
    set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME "gvplugin_${OUTPUT_NAME}")
    if (NOT DEFINED add_gvlib_TYPE OR add_gvlib_TYPE STREQUAL STATIC)
        set_target_properties(${TARGET} PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()

    install(TARGETS ${TARGET} EXPORT GraphvizTargets ${INSTALL_TARGETS_DEFAULT_ARGS})
endfunction()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(lib)
add_subdirectory(plugin)

configure_file(builddate.h.in builddate.h)
configure_file(config.h.in config.h)
configure_file(graphviz_version.h.in graphviz_version.h)
configure_file(ast_common.h.in ast_common.h) # TODO: We don't actually need this...

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/graphviz_version.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/
)
