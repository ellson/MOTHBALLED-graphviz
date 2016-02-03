add_gvlib(graphviz_lib_pathplan
    cvt.c
    inpoly.c
    route.c
    shortest.c
    shortestpth.c
    solvers.c
    triang.c
    util.c
    visibility.c
)
target_link_libraries(graphviz_lib_pathplan
    ${M_LIBRARY}
)

install(FILES
    pathgeom.h
    pathplan.h
    DESTINATION ${INCLUDE_INSTALL_DIR}
)
