# $Id$ $Revision$

INCLUDE_DIRECTORIES(
	${gvplugin_core_SRCDIR}
	${top_SRCDIR}
	${common_SRCDIR}
	${graph_SRCDIR}
	${cdt_SRCDIR}
	${pathplan_SRCDIR}
	${gvc_SRCDIR}
)

SET(gvplugin_core_SRCS
	${gvplugin_core_SRCDIR}/gvplugin_core.c
	${gvplugin_core_SRCDIR}/gvrender_core_svg.c
	${gvplugin_core_SRCDIR}/gvrender_core_ps.c
	${gvplugin_core_SRCDIR}/gvrender_core_map.c
	${gvplugin_core_SRCDIR}/gvloadimage_core.c
)
