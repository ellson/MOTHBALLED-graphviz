add_gvlib(graphviz_lib_label
    xlabels.c index.c node.c rectangle.c split.q.c
)

target_include_directories(graphviz_lib_label
    PRIVATE ../common
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(graphviz_lib_label
    graphviz_lib_cdt
    graphviz_lib_cgraph
)
