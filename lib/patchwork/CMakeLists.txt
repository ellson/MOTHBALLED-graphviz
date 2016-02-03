add_gvlib(graphviz_lib_patchwork
    patchwork.c
    patchworkinit.c
    tree_map.c
)
target_link_libraries(graphviz_lib_patchwork
    graphviz_lib_gvc
)
