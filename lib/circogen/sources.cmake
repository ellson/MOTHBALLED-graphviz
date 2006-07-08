# $Id$ $Revision$

INCLUDE_DIRECTORIES(
        ${circogen_SRCDIR}
	${top_SRCDIR}
	${common_SRCDIR}
	${gvc_SRCDIR}
	${neatogen_SRCDIR}
	${pack_SRCDIR}
	${pathplan_SRCDIR}
	${graph_SRCDIR}
	${cdt_SRCDIR}
)

SET(circogen_SRCS
	${circogen_SRCDIR}/circularinit.c
	${circogen_SRCDIR}/nodelist.c
	${circogen_SRCDIR}/block.c
	${circogen_SRCDIR}/edgelist.c
	${circogen_SRCDIR}/stack.c
	${circogen_SRCDIR}/circular.c
	${circogen_SRCDIR}/deglist.c
	${circogen_SRCDIR}/blocktree.c
	${circogen_SRCDIR}/blockpath.c
	${circogen_SRCDIR}/circpos.c
	${circogen_SRCDIR}/nodeset.c
)
