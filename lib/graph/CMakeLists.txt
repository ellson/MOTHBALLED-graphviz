include_directories(${CMAKE_CURRENT_SOURCE_DIR})
bison_with_replace(parser.y "ag")
add_gvlib(${GV_GRAPH_LIB}
    agxbuf.c
    attribs.c
    edge.c
    graph.c
    graphio.c
    lexer.c
    node.c
    refstr.c
    trie.c

    ${CMAKE_CURRENT_BINARY_DIR}/parser.c
    ${CMAKE_CURRENT_BINARY_DIR}/parser.h
)
target_link_libraries(${GV_GRAPH_LIB}
    graphviz_lib_cdt
)

install(FILES
    graph.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/
)

