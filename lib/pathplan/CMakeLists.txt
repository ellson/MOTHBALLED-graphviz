add_definitions(-D_BLD_pathplan -DPATHPLAN_EXPORTS)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(pathplan SHARED
    # Header files
    pathgeom.h
    pathplan.h
    pathutil.h
    solvers.h
    tri.h
    vis.h
    vispath.h

    # Source files
    cvt.c
    inpoly.c
    route.c
    shortest.c
    shortestpth.c
    solvers.c
    triang.c
    util.c
    visibility.c

    # Link definition file for Windows
    pathplan.def
)

# Installation location of library files
install(
    TARGETS pathplan
    RUNTIME DESTINATION ${BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIBRARY_INSTALL_DIR}
)

# Specify headers to be installed
install(
    FILES pathgeom.h pathplan.h
    DESTINATION ${HEADER_INSTALL_DIR}
)

# Specify man pages to be installed
install(
    FILES pathplan.3
    DESTINATION ${MAN_INSTALL_DIR}
)

# Specify library version and soversion
set_target_properties(pathplan PROPERTIES
    VERSION 4.0.0
    SOVERSION 4
)
