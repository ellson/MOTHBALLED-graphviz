include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GRAPHVIZ_LIB_DIR}/cdt
    ${GRAPHVIZ_LIB_DIR}/cgraph
    ${GRAPHVIZ_LIB_DIR}/common
    ${GRAPHVIZ_LIB_DIR}/gvc
    ${GRAPHVIZ_LIB_DIR}/ortho
    ${GRAPHVIZ_LIB_DIR}/pack
    ${GRAPHVIZ_LIB_DIR}/pathplan
)

add_library(dotgen STATIC
    # Header files
    aspect.h
    dot.h
    dotprocs.h

    # Source files
    aspect.c
    acyclic.c
    class1.c
    class2.c
    cluster.c
    compound.c
    conc.c
    decomp.c
    dotinit.c
    dotsplines.c
    fastgr.c
    flat.c
    mincross.c
    position.c
    rank.c
    sameport.c
)
