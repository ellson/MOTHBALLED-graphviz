if (GD_FOUND)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GRAPHVIZ_LIB_DIR}/cdt
    ${GRAPHVIZ_LIB_DIR}/cgraph
    ${GRAPHVIZ_LIB_DIR}/common
    ${GRAPHVIZ_LIB_DIR}/gvc
    ${GRAPHVIZ_LIB_DIR}/pathplan
    ${GD_INCLUDE_DIRS}
)

add_library(gvplugin_gd SHARED
    # Source files
    gvdevice_gd.c
    gvloadimage_gd.c
    gvplugin_gd.c
	gvrender_gd.c
	gvrender_gd_vrml.c
	gvtextlayout_gd.c
)

target_link_libraries(gvplugin_gd
    cdt
    cgraph
    gvc
    ${GD_LIBRARIES}
)

# Installation location of library files
install(
    TARGETS gvplugin_gd
    RUNTIME DESTINATION ${BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${PLUGIN_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIBRARY_INSTALL_DIR}
)

# Include DLLs with this library on Windows
if (WIN32)
    install(
        FILES
            ${GD_RUNTIME_LIBRARIES}
        DESTINATION ${BINARY_INSTALL_DIR}
    )
endif()

# Specify library version and soversion
set_target_properties(gvplugin_gd PROPERTIES
    VERSION ${GRAPHVIZ_PLUGIN_VERSION}.0.0
    SOVERSION ${GRAPHVIZ_PLUGIN_VERSION}
)

endif (GD_FOUND)
