add_definitions(-D_BLD_gvc=1 -DGVC_EXPORTS -DGVLIBDIR="${LIBRARY_INSTALL_DIR}/graphviz")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${TOP_SOURCE_DIR}
    ${GRAPHVIZ_LIB_DIR}/ast
    ${GRAPHVIZ_LIB_DIR}/cdt
    ${GRAPHVIZ_LIB_DIR}/cgraph
    ${GRAPHVIZ_LIB_DIR}/common
    ${GRAPHVIZ_LIB_DIR}/pathplan
    ${LTDL_INCLUDE_DIRS}
    ${RXSPENCER_INCLUDE_DIRS}
)

add_library(gvc SHARED
    # Header files
    gvc.h
    gvcext.h
    gvcint.h
    gvcjob.h
    gvconfig.h
    gvcommon.h
    gvcproc.h
    gvio.h
    gvplugin.h
    gvplugin_device.h
    gvplugin_layout.h
    gvplugin_loadimage.h
    gvplugin_render.h
    gvplugin_textlayout.h

    # Source files
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
    gvtool_tred.c
    gvusershape.c

    # Link definition file for Windows
    gvc.def
)

target_link_libraries(gvc
    cdt
    cgraph
    common
    label
    pack
    pathplan
    xdot
    ${EXPAT_LIBRARIES}
    ${RXSPENCER_LIBRARIES}
    ${ZLIB_LIBRARIES}
)

if (NOT WIN32)
    target_link_libraries(gvc ${LTDL_LIBRARIES})
endif()

if (${with_ortho})
    target_link_libraries(gvc ortho)
endif()

# Installation location of library files
install(
    TARGETS gvc
    RUNTIME DESTINATION ${BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIBRARY_INSTALL_DIR}
)

# Specify headers to be installed
install(
    FILES
        gvc.h
        gvcext.h
        gvcjob.h
        gvconfig.h
        gvcommon.h
        gvplugin.h
        gvplugin_device.h
        gvplugin_layout.h
        gvplugin_loadimage.h
        gvplugin_render.h
        gvplugin_textlayout.h
    DESTINATION ${HEADER_INSTALL_DIR}
)

# Specify man pages to be installed
install(
    FILES gvc.3
    DESTINATION ${MAN_INSTALL_DIR}
)

# Specify library version and soversion
set_target_properties(gvc PROPERTIES
    VERSION 6.0.0
    SOVERSION 6
)

# Include DLLs with this library on Windows
if (WIN32)
    install(
        FILES
            ${EXPAT_RUNTIME_LIBRARIES}
        DESTINATION ${BINARY_INSTALL_DIR}
    )
endif()
