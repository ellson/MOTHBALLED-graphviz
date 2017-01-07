include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GRAPHVIZ_LIB_DIR}/cdt
    ${GRAPHVIZ_LIB_DIR}/cgraph
    ${GRAPHVIZ_LIB_DIR}/common
    ${GRAPHVIZ_LIB_DIR}/sfdpgen
)

add_library(sparse STATIC
    # Header files
    BinaryHeap.h
    clustering.h
    color_palette.h
    colorutil.h
    DotIO.h
    general.h
    IntStack.h
    LinkedList.h
    mq.h
    QuadTree.h
    SparseMatrix.h
    vector.h

    # Source files
    BinaryHeap.c
    clustering.c
    color_palette.c
    colorutil.c
    DotIO.c
    general.c
    IntStack.c
    LinkedList.c
    mq.c
    QuadTree.c
    SparseMatrix.c
    vector.c
)
