if (ANN_FOUND)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GRAPHVIZ_LIB_DIR}/cdt
    ${GRAPHVIZ_LIB_DIR}/cgraph
    ${GRAPHVIZ_LIB_DIR}/common
    ${GRAPHVIZ_LIB_DIR}/gvc
    ${GRAPHVIZ_LIB_DIR}/pathplan
    ${GRAPHVIZ_LIB_DIR}/sfdpgen
    ${GRAPHVIZ_LIB_DIR}/sparse
    ${ANN_INCLUDE_DIR}
)

add_library(libmingle STATIC
    # Header files
    agglomerative_bundling.h
    edge_bundling.h
    ink.h
    nearest_neighbor_graph.h
    nearest_neighbor_graph_ann.h

    # Source files
    agglomerative_bundling.c
    edge_bundling.c
    ink.c
    nearest_neighbor_graph.c
    nearest_neighbor_graph_ann.cpp
)

endif (ANN_FOUND)
