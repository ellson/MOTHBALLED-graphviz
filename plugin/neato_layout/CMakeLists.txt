include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GRAPHVIZ_LIB_DIR}/cdt
    ${GRAPHVIZ_LIB_DIR}/cgraph
    ${GRAPHVIZ_LIB_DIR}/common
    ${GRAPHVIZ_LIB_DIR}/gvc
    ${GRAPHVIZ_LIB_DIR}/pathplan
)

add_library(gvplugin_neato_layout SHARED
    # Source files
	gvplugin_neato_layout.c
	gvlayout_neato_layout.c
)

target_link_libraries(gvplugin_neato_layout
    cdt
    cgraph
    circogen
    fdpgen
    gvc
    neatogen
    osage
    patchwork
    pathplan
    rbtree
    sparse
    twopigen
)

if (with_ipsepcola)
    target_link_libraries(gvplugin_neato_layout vpsc)
endif (with_ipsepcola)

if (with_sfdp)
    target_link_libraries(gvplugin_neato_layout sfdpgen)
endif (with_sfdp)

# Installation location of library files
install(
    TARGETS gvplugin_neato_layout
    RUNTIME DESTINATION ${BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${PLUGIN_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIBRARY_INSTALL_DIR}
)

# Specify library version and soversion
set_target_properties(gvplugin_neato_layout PROPERTIES
    VERSION ${GRAPHVIZ_PLUGIN_VERSION}.0.0
    SOVERSION ${GRAPHVIZ_PLUGIN_VERSION}
)
