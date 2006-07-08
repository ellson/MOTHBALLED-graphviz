# $Id$ $Revision$

INCLUDE_DIRECTORIES(
	${dotgen_SRCDIR}
	${top_SRCDIR}
	${common_SRCDIR}
	${gvc_SRCDIR}
	${graph_SRCDIR}
	${cdt_SRCDIR}
	${pathplan_SRCDIR}
)

SET(dotgen_SRCS
	${dotgen_SRCDIR}/acyclic.c
	${dotgen_SRCDIR}/class1.c
	${dotgen_SRCDIR}/class2.c
	${dotgen_SRCDIR}/cluster.c
	${dotgen_SRCDIR}/compound.c
	${dotgen_SRCDIR}/conc.c
	${dotgen_SRCDIR}/decomp.c
	${dotgen_SRCDIR}/fastgr.c
	${dotgen_SRCDIR}/flat.c
	${dotgen_SRCDIR}/dotinit.c
	${dotgen_SRCDIR}/mincross.c
	${dotgen_SRCDIR}/position.c
	${dotgen_SRCDIR}/rank.c
	${dotgen_SRCDIR}/sameport.c
	${dotgen_SRCDIR}/dotsplines.c
)
