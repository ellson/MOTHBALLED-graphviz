# $Id$ $Revision$

INCLUDE_DIRECTORIES(
	${gvplugin_xlib_SRCDIR}
	${top_SRCDIR}
	${common_SRCDIR}
	${graph_SRCDIR}
	${cdt_SRCDIR}
	${pathplan_SRCDIR}
	${gvc_SRCDIR}
)

SET(gvplugin_xlib_SRCS
	${gvplugin_xlib_SRCDIR}/gvdevice_xlib.c
	${gvplugin_xlib_SRCDIR}/gvplugin_xlib.c
)
