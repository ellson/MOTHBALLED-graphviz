include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GRAPHVIZ_LIB_DIR}/cdt
    ${GRAPHVIZ_LIB_DIR}/cgraph
    ${GRAPHVIZ_LIB_DIR}/common
    ${GRAPHVIZ_LIB_DIR}/gvc
    ${GRAPHVIZ_LIB_DIR}/ortho
    ${GRAPHVIZ_LIB_DIR}/pack
    ${GRAPHVIZ_LIB_DIR}/pathplan
    ${GRAPHVIZ_LIB_DIR}/rbtree
    ${GRAPHVIZ_LIB_DIR}/sfdpgen
    ${GRAPHVIZ_LIB_DIR}/sparse
)

set(sources
    # Header files
    adjust.h
    bfs.h
    call_tri.h
    closest.h
    conjgrad.h
    defs.h
    delaunay.h
    digcola.h
    dijkstra.h
    edges.h
    embed_graph.h
    fPQ.h
    geometry.h
    heap.h
    hedges.h
    info.h
    kkutils.h
    matrix_ops.h
    mem.h
    multispline.h
    neato.h
    neatoprocs.h
    overlap.h
    pca.h
    poly.h
    quad_prog_solver.h
    quad_prog_vpsc.h
    site.h
    sparsegraph.h
    stress.h
    voronoi.h

    # Source files
    adjust.c
    bfs.c
    call_tri.c
    circuit.c
    closest.c
    compute_hierarchy.c
    conjgrad.c
    constrained_majorization.c
    constraint.c
    delaunay.c
    dijkstra.c
    edges.c
    embed_graph.c
    geometry.c
    heap.c
    hedges.c
    info.c
    kkutils.c
    legal.c
    lu.c
    matinv.c
    matrix_ops.c
    memory.c
    multispline.c
    neatoinit.c
    neatosplines.c
    opt_arrangement.c
    overlap.c
    pca.c
    poly.c
    printvis.c
    quad_prog_solve.c
    site.c
    smart_ini_x.c
    solve.c
    stuff.c
    stress.c
    voronoi.c
)

if (with_ipsepcola)
    list(APPEND ${sources}
        constrained_majorization_ipsep.c
	    mosek_quad_solve.c
        mosek_quad_solve.h
        quad_prog_vpsc.c
    )
    include_directories(${GRAPHVIZ_LIB_DIR}/vpsc)
endif (with_ipsepcola)


add_library(neatogen STATIC
    ${sources}
)
