add_gvlib(graphviz_lib_circogen
    circularinit.c nodelist.c block.c edgelist.c
    circular.c deglist.c blocktree.c blockpath.c
    circpos.c nodeset.c
)
target_link_libraries(graphviz_lib_circogen
    graphviz_lib_gvc
    graphviz_lib_sparse
)
