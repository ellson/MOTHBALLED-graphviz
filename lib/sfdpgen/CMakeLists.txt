if (with_sfdp)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GRAPHVIZ_LIB_DIR}/cdt
    ${GRAPHVIZ_LIB_DIR}/cgraph
    ${GRAPHVIZ_LIB_DIR}/common
    ${GRAPHVIZ_LIB_DIR}/gvc
    ${GRAPHVIZ_LIB_DIR}/neatogen
    ${GRAPHVIZ_LIB_DIR}/pack
    ${GRAPHVIZ_LIB_DIR}/pathplan
    ${GRAPHVIZ_LIB_DIR}/rbtree
    ${GRAPHVIZ_LIB_DIR}/sparse
)

add_library(sfdpgen STATIC
    # Header files
    Multilevel.h
    post_process.h
    PriorityQueue.h
    sfdp.h
    sfdpinternal.h
    sparse_solve.h
    spring_electrical.h
    stress_model.h
    uniform_stress.h

    # Source files
    Multilevel.c
    post_process.c
    PriorityQueue.c
    sfdpinit.c
    sparse_solve.c
    spring_electrical.c
    stress_model.c
    uniform_stress.c
)

endif (with_sfdp)