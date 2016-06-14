add_gvlib(graphviz_lib_sfdpgen
    sfdpinit.c spring_electrical.c
    sparse_solve.c post_process.c
    stress_model.c uniform_stress.c
    Multilevel.c PriorityQueue.c
)
target_link_libraries(graphviz_lib_sfdpgen
    graphviz_lib_gvc
    graphviz_lib_sparse
)
