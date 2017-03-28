add_definitions(-DEXPORT_CDT)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(cdt SHARED
    # Header files
    cdt.h
    dthdr.h

    # Source files
    dtclose.c
    dtdisc.c
    dtextract.c
    dtflatten.c
    dthash.c
    dtlist.c
    dtmethod.c
    dtopen.c
    dtrenew.c
    dtrestore.c
    dtsize.c
    dtstat.c
    dtstrhash.c
    dttree.c
    dtview.c
    dtwalk.c
)

# Installation location of library files
install(
    TARGETS cdt
    RUNTIME DESTINATION ${BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIBRARY_INSTALL_DIR}
)

# Specify headers to be installed
install(
    FILES cdt.h
    DESTINATION ${HEADER_INSTALL_DIR}
)

# Specify man pages to be installed
install(
    FILES cdt.3
    DESTINATION ${MAN_INSTALL_DIR}
)

# Specify library version and soversion
set_target_properties(cdt PROPERTIES
    VERSION 5.0.0
    SOVERSION 5
)
