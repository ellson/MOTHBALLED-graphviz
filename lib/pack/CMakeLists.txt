add_definitions(-DGVC_EXPORTS)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GRAPHVIZ_LIB_DIR}/cdt
    ${GRAPHVIZ_LIB_DIR}/cgraph
    ${GRAPHVIZ_LIB_DIR}/common
    ${GRAPHVIZ_LIB_DIR}/gvc
    ${GRAPHVIZ_LIB_DIR}/neatogen
    ${GRAPHVIZ_LIB_DIR}/pathplan
)

add_library(pack STATIC
    # Header files
    pack.h

    # Source files
    ccomps.c
    pack.c
)

# Specify headers to be installed
install(
    FILES pack.h
    DESTINATION ${HEADER_INSTALL_DIR}
)

# Specify man pages to be installed
install(
    FILES pack.3
    DESTINATION ${MAN_INSTALL_DIR}
)
