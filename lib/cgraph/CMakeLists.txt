flex_target(scan.pre.c scan.l ${CMAKE_CURRENT_BINARY_DIR}/scan.pre.c)
add_custom_command(OUTPUT scan.c
    COMMAND sed "s/yy/aag/g" scan.pre.c | sed "/extern.*isatty/d" > scan.c
    DEPENDS scan.pre.c
)
bison_with_replace(grammar.y "aag")
add_gvlib(graphviz_lib_cgraph
    apply.c
    agerror.c
    agxbuf.c
    attr.c
    edge.c
    flatten.c
    graph.c
    id.c
    imap.c
    io.c
    mem.c
    node.c
    obj.c
    pend.c
    rec.c
    refstr.c
    scan.l
    subg.c
    utils.c
    write.c

    ${CMAKE_CURRENT_BINARY_DIR}/grammar.h
    ${CMAKE_CURRENT_BINARY_DIR}/grammar.c
    ${CMAKE_CURRENT_BINARY_DIR}/scan.c
)
if (WIN32)
    add_definitions("-DYY_NO_UNISTD_H")
endif()
target_link_libraries(graphviz_lib_cgraph
    graphviz_lib_cdt
)

install(FILES
    cgraph.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/
)
