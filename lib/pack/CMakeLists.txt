add_gvlib(graphviz_lib_pack
    ccomps.c
    pack.c
)
target_link_libraries(graphviz_lib_pack
    graphviz_lib_common
    graphviz_lib_xdot
)

install(FILES
    pack.h
    DESTINATION ${INCLUDE_INSTALL_DIR}
)
