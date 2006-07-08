# $Id$ $Revision$

INCLUDE_DIRECTORIES(
	${pack_SRCDIR}
	${top_SRCDIR}
	${common_SRCDIR}
	${gvc_SRCDIR}
	${neatogen_SRCDIR}
	${pathplan_SRCDIR}
	${graph_SRCDIR}
	${cdt_SRCDIR}
)

SET(pack_SRCS
	${pack_SRCDIR}/ccomps.c
	${pack_SRCDIR}/pack.c
)
