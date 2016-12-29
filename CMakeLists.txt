cmake_minimum_required (VERSION 2.8 FATAL_ERROR)

# Required to set build version
find_package(Git REQUIRED)

# General project information
# ===========================
project (Graphviz)

set(GRAPHVIZ_VERSION_MAJROR 2)
set(GRAPHVIZ_VERSION_MINOR 39)

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

# Build dependencies
# ==================
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# Convenient values for CMake configuration
# =========================================
set(BINARY_INSTALL_DIR bin)
set(LIBRARY_INSTALL_DIR lib)
set(HEADER_INSTALL_DIR include/graphviz)
set(MAN_INSTALL_DIR share/man/man3)
# TODO: Find a way to check for groff and ps2pdf for manpage pdf generation
# set(MAN_PDF_INSTALL_DIR share/graphviz/doc/pdf)


# Packaging information
# =====================
set(CPACK_GENERATOR ZIP)

include(CPack)

add_subdirectory(lib)
