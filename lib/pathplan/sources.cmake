# $Id$ $Revision$

INCLUDE_DIRECTORIES(
	${pathplan_SRCDIR}
	${top_SRCDIR}
)

SET(pathplan_SRCS
	${pathplan_SRCDIR}/cvt.c
	${pathplan_SRCDIR}/inpoly.c
	${pathplan_SRCDIR}/route.c
	${pathplan_SRCDIR}/shortest.c
	${pathplan_SRCDIR}/shortestpth.c
	${pathplan_SRCDIR}/solvers.c
	${pathplan_SRCDIR}/triang.c
	${pathplan_SRCDIR}/util.c
	${pathplan_SRCDIR}/visibility.c
)
