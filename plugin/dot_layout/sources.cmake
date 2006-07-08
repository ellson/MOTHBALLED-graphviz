# $Id$ $Revision$

INCLUDE_DIRECTORIES(
	${gvplugin_dot_layout_SRCDIR}
        ${top_SRCDIR}
	${common_SRCDIR}
        ${graph_SRCDIR}
	${cdt_SRCDIR}
	${pathplan_SRCDIR}
	${gvc_SRCDIR}
)

SET(gvplugin_dot_layout_SRCS
	${gvplugin_dot_layout_SRCDIR}/gvplugin_dot_layout.c
	${gvplugin_dot_layout_SRCDIR}/gvlayout_dot_layout.c
)
