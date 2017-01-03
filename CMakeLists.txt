cmake_minimum_required (VERSION 2.8 FATAL_ERROR)
project (Graphviz)

# Append local CMake module directory
# ===================================
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Build dependencies
# ==================
find_package(Git REQUIRED)
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# Convenient values for CMake configuration
# =========================================
set(BINARY_INSTALL_DIR  bin)
set(LIBRARY_INSTALL_DIR lib)
set(HEADER_INSTALL_DIR  include/graphviz)
set(MAN_INSTALL_DIR     share/man/man3)
# TODO: Find a way to check for groff and ps2pdf for manpage pdf generation
# set(MAN_PDF_INSTALL_DIR share/graphviz/doc/pdf)
set(TOP_SOURCE_DIR      "${CMAKE_SOURCE_DIR}")
set(GRAPHVIZ_LIB_DIR    "${CMAKE_CURRENT_SOURCE_DIR}/lib")

# Set Graphviz version
# ====================
set(GRAPHVIZ_VERSION_MAJROR 2)
set(GRAPHVIZ_VERSION_MINOR 41)

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

message(STATUS "Graphviz version: ${GRAPHVIZ_VERSION_MAJROR}.${GRAPHVIZ_VERSION_MINOR}.${GRAPHVIZ_VERSION_BUILD}")

# Check avaiable headers, functions and libraries and write the result to config.h
# ================================================================================
include(config_checks)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Custom target for `make uninstall`
# ==================================
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

# Packaging information
# =====================
set(CPACK_GENERATOR ZIP)
include(CPack)

# Specify subdirectories to build
# ===============================
add_subdirectory(lib)
