add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ps.h
    COMMAND awk -f ${Graphviz_SOURCE_DIR}/awk/stringize.awk ${CMAKE_CURRENT_SOURCE_DIR}/ps.txt > ps.h
    DEPENDS ps.txt
)

add_gvplugin(graphviz_plugin_core
    gvloadimage_core.c
    gvplugin_core.c
    gvrender_core_dot.c
    gvrender_core_fig.c
    gvrender_core_map.c
    gvrender_core_ps.c
    gvrender_core_svg.c
    gvrender_core_tk.c
    gvrender_core_vml.c
    gvrender_core_pov.c
    gvrender_core_json.c
    gvrender_core_pic.c

    ${CMAKE_CURRENT_BINARY_DIR}/ps.h
)
target_link_libraries(graphviz_plugin_core
    graphviz_lib_common
    graphviz_lib_gvc
    ${GV_GRAPH_LIB}
    graphviz_lib_xdot
)
