add_definitions(-DGVLIBDIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")

# We build this library as static since this depends on libcommon, which
# depends on us (i.e. there is a cyclic dependency).
set(GVC_SRCS
    gvc.c
    gvconfig.c
    gvcontext.c
    gvdevice.c
    gvevent.c
    gvjobs.c
    gvlayout.c
    gvloadimage.c
    gvplugin.c
    gvrender.c
    gvtextlayout.c
    gvusershape.c
    #no_demand_loading.c
)
if (WIN32)
    list(APPEND GVC_SRCS regex_win32.c)
endif()
add_gvlib(graphviz_lib_gvc ${GVC_SRCS})
target_link_libraries(graphviz_lib_gvc
    graphviz_lib_common
    graphviz_lib_cdt
    graphviz_lib_cgraph
)

install(FILES
    gvc.h
    gvcext.h
    gvcjob.h
    gvcommon.h
    gvconfig.h
    gvplugin_device.h
    gvplugin.h
    gvplugin_layout.h
    gvplugin_loadimage.h
    gvplugin_render.h
    gvplugin_textlayout.h
    DESTINATION ${INCLUDE_INSTALL_DIR}
)
