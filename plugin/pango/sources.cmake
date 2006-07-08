# $Id$ $Revision$

INCLUDE( UsePkgConfig )
PKGCONFIG( pangocairo PANGOCAIRO_INCLUDE_DIR PANGOCAIRO_LIB_DIR PANGOCAIRO_LINK_FLAGS PANGOCAIRO_CFLAGS )
ADD_DEFINITIONS(${PANGOCAIRO_CFLAGS})

INCLUDE_DIRECTORIES(
	${gvplugin_pango_SRCDIR}
	${top_SRCDIR}
	${common_SRCDIR}
	${graph_SRCDIR}
	${cdt_SRCDIR}
	${pathplan_SRCDIR}
	${gvc_SRCDIR}
)

SET(gvplugin_pango_SRCS
	${gvplugin_pango_SRCDIR}/gvplugin_pango.c
	${gvplugin_pango_SRCDIR}/gvrender_pango.c
	${gvplugin_pango_SRCDIR}/gvtextlayout_pango.c
	${gvplugin_pango_SRCDIR}/gvloadimage_pango.c
)
