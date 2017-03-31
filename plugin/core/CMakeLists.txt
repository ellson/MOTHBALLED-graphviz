include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${TOP_SOURCE_DIR}
    ${GRAPHVIZ_LIB_DIR}/ast
    ${GRAPHVIZ_LIB_DIR}/cdt
    ${GRAPHVIZ_LIB_DIR}/cgraph
    ${GRAPHVIZ_LIB_DIR}/common
    ${GRAPHVIZ_LIB_DIR}/expr
    ${GRAPHVIZ_LIB_DIR}/gvc
    ${GRAPHVIZ_LIB_DIR}/gvpr
    ${GRAPHVIZ_LIB_DIR}/pathplan
    ${GRAPHVIZ_LIB_DIR}/sfio
    ${GRAPHVIZ_LIB_DIR}/vmalloc
    ${GRAPHVIZ_LIB_DIR}/xdot
)

# Generate ps.h from ps.txt
add_custom_command(
    OUTPUT ps.h
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ps.txt
    COMMAND ${AWK_EXECUTABLE} -f ${TOP_SOURCE_DIR}/awk/stringize.awk
            ${CMAKE_CURRENT_SOURCE_DIR}/ps.txt > ${CMAKE_CURRENT_BINARY_DIR}/ps.h
)

add_library(gvplugin_core SHARED
    # Header files
    ps.h

    # Source files
	gvloadimage_core.c
	gvplugin_core.c
	gvrender_core_dot.c
	gvrender_core_json.c
	gvrender_core_fig.c
	gvrender_core_map.c
	gvrender_core_mp.c
	gvrender_core_pic.c
	gvrender_core_pov.c
	gvrender_core_ps.c
	gvrender_core_svg.c
	gvrender_core_tk.c
	gvrender_core_vml.c
)

target_link_libraries(gvplugin_core
    cdt
    cgraph
    gvc
)

# Installation location of library files
install(
    TARGETS gvplugin_core
    RUNTIME DESTINATION ${BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${PLUGIN_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIBRARY_INSTALL_DIR}
)

# Specify library version and soversion
set_target_properties(gvplugin_core PROPERTIES
    VERSION ${GRAPHVIZ_PLUGIN_VERSION}.0.0
    SOVERSION ${GRAPHVIZ_PLUGIN_VERSION}
)
