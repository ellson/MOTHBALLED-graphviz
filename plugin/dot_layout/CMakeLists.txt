add_gvplugin(graphviz_plugin_dot_layout
    gvlayout_dot_layout.c
    gvplugin_dot_layout.c
)

target_link_libraries(graphviz_plugin_dot_layout
    graphviz_lib_gvc
    graphviz_lib_dotgen
)
