# $Id$ $Revision$

INCLUDE_DIRECTORIES(
	${top_SRCDIR}
	${agutil_SRCDIR}
	${agraph_SRCDIR}
	${cdt_SRCDIR}
)

SET(agutil_SRCS
	${agutil_SRCDIR}/dynattr.c
	${agutil_SRCDIR}/nodeq.c
)
