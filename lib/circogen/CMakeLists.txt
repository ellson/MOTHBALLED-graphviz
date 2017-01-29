include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GRAPHVIZ_LIB_DIR}/cdt
    ${GRAPHVIZ_LIB_DIR}/cgraph
    ${GRAPHVIZ_LIB_DIR}/common
    ${GRAPHVIZ_LIB_DIR}/gvc
    ${GRAPHVIZ_LIB_DIR}/neatogen
    ${GRAPHVIZ_LIB_DIR}/pack
    ${GRAPHVIZ_LIB_DIR}/pathplan
    ${GRAPHVIZ_LIB_DIR}/sparse
)

add_library(circogen STATIC
    # Header files
    block.h
    blockpath.h
    blocktree.h
    circo.h
    circpos.h
    circular.h
    deglist.h
    edgelist.h
    nodelist.h
    nodeset.h

    # Source files
    block.c
    blockpath.c
    blocktree.c
    circpos.c
    circular.c
    circularinit.c
    deglist.c
    edgelist.c
    nodelist.c
    nodeset.c
)
