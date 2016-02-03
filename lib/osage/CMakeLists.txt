add_gvlib(graphviz_lib_osage
    osageinit.c
)

target_link_libraries(graphviz_lib_osage
    graphviz_lib_common
    graphviz_lib_pack
    graphviz_lib_neatogen
)
