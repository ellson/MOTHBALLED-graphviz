add_gvlib(graphviz_lib_dotgen
    acyclic.c class1.c class2.c cluster.c compound.c
    conc.c decomp.c fastgr.c flat.c dotinit.c mincross.c
    position.c rank.c sameport.c dotsplines.c aspect.c
)
target_link_libraries(graphviz_lib_dotgen
    graphviz_lib_gvc
)
