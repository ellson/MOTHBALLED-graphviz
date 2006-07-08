# $Id$ $Revision$

INCLUDE_DIRECTORIES(
        ${gvplugin_neato_layout_SRCDIR}
	${top_SRCDIR}
	${common_SRCDIR}
	${graph_SRCDIR}
	${cdt_SRCDIR}
	${pathplan_SRCDIR}
	${gvc_SRCDIR}
)

SET(gvplugin_neato_layout_SRCS
	${gvplugin_neato_layout_SRCDIR}/gvplugin_neato_layout.c
	${gvplugin_neato_layout_SRCDIR}/gvlayout_neato_layout.c
)
