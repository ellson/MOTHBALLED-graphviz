add_gvlib(graphviz_lib_sparse
    general.c
    BinaryHeap.c
    IntStack.c
    SparseMatrix.c
    QuadTree.c
    LinkedList.c

    vector.c
    DotIO.c
    colorutil.c
    color_palette.c
    mq.c
    clustering.c
    QuadTree.c
)
target_include_directories(graphviz_lib_sparse
    PRIVATE ../common
)
target_link_libraries(graphviz_lib_sparse
    graphviz_lib_cgraph
    ${M_LIBRARY}
)
