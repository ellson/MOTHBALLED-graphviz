# $Id$ $Revision$

INCLUDE_DIRECTORIES(
	${twopigen_SRCDIR}
	${top_SRCDIR}
	${common_SRCDIR}
	${gvc_SRCDIR}
	${neatogen_SRCDIR}
	${pack_SRCDIR}
	${pathplan_SRCDIR}
	${graph_SRCDIR}
	${cdt_SRCDIR}
)

SET(twopigen_SRCS
	${twopigen_SRCDIR}/circle.c
	${twopigen_SRCDIR}/twopiinit.c
)
