include_directories(
    # We need to add these includes before the existing ones, as neatogen
    # also include fPQ.h (but an incompatible version).
    BEFORE
    ${Graphviz_SOURCE_DIR}/lib/ortho
    ${Graphviz_SOURCE_DIR}
)

add_gvlib(graphviz_lib_ortho
    fPQ.c
    fPQ.h
    maze.c
    maze.h
    ortho.c
    ortho.h
    partition.c
    partition.h
    rawgraph.c
    rawgraph.h
    sgraph.c
    sgraph.h
    structures.h
    trap.h
    trapezoid.c
)
target_include_directories(graphviz_lib_ortho
    PRIVATE ../gvc
)
target_link_libraries(graphviz_lib_ortho
    graphviz_lib_common
    graphviz_lib_pathplan
    graphviz_lib_cgraph
)
