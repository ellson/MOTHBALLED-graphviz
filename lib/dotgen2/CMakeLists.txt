add_gvlib(graphviz_lib_dotgen2
    dotinit.c level.c minc2.c ns.c groups.c minc_utils.c decomp.c
)
target_link_libraries(graphviz_lib_dotgen2
    graphviz_lib_common
    graphviz_lib_cdt
)

add_executable(graphviz_bin_dotgen2 main.c)
target_link_libraries(graphviz_bin_dotgen2
    graphviz_lib_dotgen2
    graphviz_lib_cgraph
)
