cmake_minimum_required (VERSION 2.8 FATAL_ERROR)
project (Graphviz)

include(FeatureSummary)

# =============================== Build options ================================
option(enable_ltdl     "Support on-demand plugin loading" ON)
option(with_digcola    "DIGCOLA features in neato layout engine" ON )
option(with_ipsepcola  "IPSEPCOLA features in neato layout engine (disabled by default - C++ portability issues)." OFF )
option(with_ortho      "ORTHO features in neato layout engine." ON )
option(with_sfdp       "sfdp layout engine." ON )
option(with_smyrna     "SMYRNA large graph viewer (disabled by default - experimental)" OFF)

if (enable_ltdl)
        add_definitions(-DENABLE_LTDL)
endif (enable_ltdl)

if (with_digcola)
    add_definitions(-DDIGCOLA)
endif (with_digcola)

if (with_ipsepcola)
    add_definitions(-DIPSEPCOLA)
endif (with_ipsepcola)

if (with_ortho)
    add_definitions(-DORTHO)
endif (with_ortho)

if (with_sfdp)
    add_definitions(-DSFDP)
endif (with_sfdp)

if (with_smyrna)
    # TODO include dependency checks
    add_definitions(-DSMYRNA)
endif (with_smyrna)

# ===================== Append local CMake module directory ====================
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# ============================= Build dependencies =============================
find_package(AWK REQUIRED)
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
find_package(Git REQUIRED)
find_package(Perl REQUIRED)

# ================== Convenient values for CMake configuration =================
set(BINARY_INSTALL_DIR      bin)
set(LIBRARY_INSTALL_DIR     lib)
set(PLUGIN_INSTALL_DIR      ${LIBRARY_INSTALL_DIR}/graphviz)
set(HEADER_INSTALL_DIR      include/graphviz)
set(MAN_INSTALL_DIR         share/man/man3)
# TODO: Find a way to check for groff and ps2pdf for manpage pdf generation
# set(MAN_PDF_INSTALL_DIR share/graphviz/doc/pdf)
set(TOP_SOURCE_DIR          "${CMAKE_SOURCE_DIR}")
set(TOP_BINARY_DIR          "${CMAKE_CURRENT_BINARY_DIR}")
set(GRAPHVIZ_LIB_DIR        "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(WINDOWS_DEPENDENCY_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/windows/dependencies/libraries")
if (CMAKE_CL_64)
    set(WINDOWS_DEPENDENCY_DIR "${WINDOWS_DEPENDENCY_DIR}/x64")
else()
    set(WINDOWS_DEPENDENCY_DIR "${WINDOWS_DEPENDENCY_DIR}/x86")
endif()

set(GRAPHVIZ_PLUGIN_VERSION 6)
# Name of the config file used by Graphviz
set(GVPLUGIN_CONFIG_FILE config${GRAPHVIZ_PLUGIN_VERSION})

# ============================ Library dependencies ============================
if (WIN32)
    list(APPEND CMAKE_LIBRARY_PATH ${WINDOWS_DEPENDENCY_DIR}/lib)
    list(APPEND CMAKE_INCLUDE_PATH ${WINDOWS_DEPENDENCY_DIR}/include ${WINDOWS_DEPENDENCY_DIR}/bin)
else()
    list(APPEND CMAKE_INCLUDE_PATH /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu)
endif()

find_package(ANN)
find_package(Cairo)
find_package(EXPAT)
find_package(GD)
find_package(LTDL)
find_package(PangoCairo)
find_package(ZLIB)

if (WIN32)
    # Find Windows specific dependencies
    find_package(RxSpencer REQUIRED)

    # Find DLLs on Windows
    find_file(EXPAT_RUNTIME_LIBRARIES NAMES libexpat.dll expat.dll)
    if (CMAKE_CL_64)
        find_file(MSYS_RUNTIME_LIBRARIES NAMES msys-2.0.dll)
    endif()
endif()

# ============================ Set Graphviz version ============================
set(GRAPHVIZ_VERSION_MAJOR 2)
set(GRAPHVIZ_VERSION_MINOR 41)
set(GRAPHVIZ_VERSION_PATCH 0)
set(GRAPHVIZ_VERSION_STRING "${GRAPHVIZ_VERSION_MAJOR}.${GRAPHVIZ_VERSION_MINOR}.${GRAPHVIZ_VERSION_PATCH}")
configure_file(graphviz_version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/graphviz_version.h @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/graphviz_version.h DESTINATION ${HEADER_INSTALL_DIR})

# Set GRAPHVIZ_VERSION_BUILD to time of last commit, or to 0 if that fails.
execute_process(
    COMMAND           "${GIT_EXECUTABLE}" log -n 1 --format=%cd --date=format:%Y%m%d.%H%M
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    RESULT_VARIABLE   git_result
    OUTPUT_VARIABLE   GRAPHVIZ_VERSION_BUILD
    ERROR_VARIABLE    git_error
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_STRIP_TRAILING_WHITESPACE
)
if(NOT git_result EQUAL 0)
    message(WARNING "Failed to set build version: ${git_error}")
    set(GRAPHVIZ_VERSION_BUILD 0)
endif()

set(GRAPHVIZ_VERSION_FULL "${GRAPHVIZ_VERSION_MAJOR}.${GRAPHVIZ_VERSION_MINOR}.${GRAPHVIZ_VERSION_BUILD}")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/builddate.h "#define BUILDDATE \"${GRAPHVIZ_VERSION_BUILD}\"")

message(STATUS "Graphviz version: ${GRAPHVIZ_VERSION_FULL}")

# =================== Generate config.h with proper defines ====================
# This check for getopt is used later in the build, so it is done here.
include(CheckIncludeFile)
check_include_file(getopt.h HAVE_GETOPT_H)

include(config_checks)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# ==================== Custom target for `make uninstall` ======================
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

# =========================== Compiler configuration ===========================
if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    # MSVC warning C4996 mostly fires on completely valid code. The changes
    # proposed in the warning text often seriously compromise the code
    # portability, while they never substantially improve the code quality. Thus
    # we suppress this warning.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4996")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4996")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

# ============================ Packaging information ===========================
include(InstallRequiredSystemLibraries)
include(package_info)
include(CPack)

# ======================= Specify subdirectories to build ======================
add_subdirectory(windows)
add_subdirectory(lib)
add_subdirectory(plugin)
add_subdirectory(cmd)

enable_testing()
add_subdirectory(tests)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
