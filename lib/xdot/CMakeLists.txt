add_definitions(-DEXPORT_XDOT)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(xdot SHARED
    # Header files
    xdot.h

    # Source files
    xdot.c
)

# Installation location of library files
install(
    TARGETS xdot
    RUNTIME DESTINATION ${BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIBRARY_INSTALL_DIR}
)

# Specify headers to be installed
install(
    FILES xdot.h
    DESTINATION ${HEADER_INSTALL_DIR}
)

# Specify man pages to be installed
install(
    FILES xdot.3
    DESTINATION ${MAN_INSTALL_DIR}
)

# Specify library version and soversion
set_target_properties(xdot PROPERTIES
    VERSION 4.0.0
    SOVERSION 4
)
