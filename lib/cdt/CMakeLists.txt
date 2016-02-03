add_gvlib(graphviz_lib_cdt
    dttree.c
    dtclose.c
    dtdisc.c
    dtextract.c
    dtflatten.c
    dthash.c
    dtlist.c
    dtmethod.c
    dtopen.c
    dtrenew.c
    dtrestore.c
    dtsize.c
    dtstat.c
    dtstrhash.c
    dtview.c
    dtwalk.c
)

install(FILES
    cdt.h
    DESTINATION ${INCLUDE_INSTALL_DIR}
)
