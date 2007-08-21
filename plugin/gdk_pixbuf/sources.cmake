# $Id$ $Revision$

INCLUDE( UsePkgConfig )
PKGCONFIG( pangocairo PANGOCAIRO_INCLUDE_DIR PANGOCAIRO_LIB_DIR PANGOCAIRO_LINK_FLAGS PANGOCAIRO_CFLAGS )
ADD_DEFINITIONS(${PANGOCAIRO_CFLAGS})
PKGCONFIG( gdk-pixbuf-2.0 GDK_PIXBUF_INCLUDE_DIR GDK_PIXBUF_LIB_DIR GDK_PIXBUF_LINK_FLAGS GDK_PIXBUF_CFLAGS )
ADD_DEFINITIONS(${GDK_PIXBUF_CFLAGS})

INCLUDE_DIRECTORIES(
	${gvplugin_gdk_pixbuf_SRCDIR}
	${top_SRCDIR}
	${common_SRCDIR}
	${graph_SRCDIR}
	${cdt_SRCDIR}
	${pathplan_SRCDIR}
	${gvc_SRCDIR}
)

SET(gvplugin_gdk_pixbuf_SRCS
	${gvplugin_gdk_pixbuf_SRCDIR}/gvplugin_gdk_pixbuf.c
	${gvplugin_gdk_pixbuf_SRCDIR}/gvformatter_gdk_pixbuf.c
)
