if (WIN32)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GRAPHVIZ_LIB_DIR}/cdt
    ${GRAPHVIZ_LIB_DIR}/cgraph
    ${GRAPHVIZ_LIB_DIR}/common
    ${GRAPHVIZ_LIB_DIR}/gvc
    ${GRAPHVIZ_LIB_DIR}/pathplan
)

add_library(gvplugin_gdiplus SHARED
    # Header files
    FileStream.h
    gvplugin_gdiplus.h

    # Source files
    FileStream.cpp
	gvdevice_gdiplus.cpp
	gvloadimage_gdiplus.cpp
	gvplugin_gdiplus.cpp
	gvrender_gdiplus.cpp
	gvtextlayout_gdiplus.cpp
)

target_link_libraries(gvplugin_gdiplus
    gvc
    gdiplus
)

# Installation location of library files
install(
    TARGETS gvplugin_gdiplus
    RUNTIME DESTINATION ${BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${PLUGIN_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIBRARY_INSTALL_DIR}
)

# Specify library version and soversion
set_target_properties(gvplugin_gdiplus PROPERTIES
    VERSION ${GRAPHVIZ_PLUGIN_VERSION}.0.0
    SOVERSION ${GRAPHVIZ_PLUGIN_VERSION}
)

endif (WIN32)
