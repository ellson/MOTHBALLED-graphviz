add_definitions(-DDEMAND_LOADING=1)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GRAPHVIZ_LIB_DIR}/cdt
    ${GRAPHVIZ_LIB_DIR}/cgraph
    ${GRAPHVIZ_LIB_DIR}/common
    ${GRAPHVIZ_LIB_DIR}/gvc
    ${GRAPHVIZ_LIB_DIR}/pathplan
)

add_executable(dot
    # Source files
    dot.c
    no_builtins.c
)

target_link_libraries(dot
    cgraph
    gvc
)

# Link to math library
if (UNIX)
    target_link_libraries(dot m)
endif (UNIX)

# Installation location of executables
install(
    TARGETS dot
    RUNTIME DESTINATION ${BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIBRARY_INSTALL_DIR}
)

# Specify man pages to be installed
install(
    FILES dot.1 osage.1 patchwork.1
    DESTINATION ${MAN_INSTALL_DIR}
)
