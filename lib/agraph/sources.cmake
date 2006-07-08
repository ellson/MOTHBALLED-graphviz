# $Id$ $Revision$

INCLUDE_DIRECTORIES(
	${top_SRCDIR}
	${agraph_SRCDIR}
	${cdt_SRCDIR}
)

ADD_CUSTOM_COMMAND(
	WORKING_DIRECTORY ${agraph_SRCDIR}
	OUTPUT grammar.c
	OUTPUT grammar.h
	DEPENDS grammar.y
	COMMAND yacc -dv grammar.y
	COMMAND sed "s/yy/aag/g" < y.tab.c > grammar.c
	COMMAND sed "s/yy/aag/g" < y.tab.h > grammar.h
	COMMAND rm y.tab.c y.tab.h y.output
)

ADD_CUSTOM_COMMAND(
	WORKING_DIRECTORY ${agraph_SRCDIR}
	OUTPUT scan.c
	DEPENDS scan.l
	COMMAND flex -i scan.l
	COMMAND sed "s/yy/aag/g" < lex.yy.c > scan.c
	COMMAND rm lex.yy.c
)

SET(agraph_SRCS
	${agraph_SRCDIR}/agerror.c
	${agraph_SRCDIR}/apply.c
	${agraph_SRCDIR}/attr.c
	${agraph_SRCDIR}/edge.c
	${agraph_SRCDIR}/flatten.c
	${agraph_SRCDIR}/graph.c
	${agraph_SRCDIR}/id.c
	${agraph_SRCDIR}/imap.c
	${agraph_SRCDIR}/io.c
	${agraph_SRCDIR}/mem.c
	${agraph_SRCDIR}/node.c
	${agraph_SRCDIR}/obj.c
	${agraph_SRCDIR}/pend.c
	${agraph_SRCDIR}/rec.c
	${agraph_SRCDIR}/refstr.c
	${agraph_SRCDIR}/subg.c
	${agraph_SRCDIR}/utils.c
	${agraph_SRCDIR}/write.c
	${agraph_SRCDIR}/grammar.c
	${agraph_SRCDIR}/grammar.h
	${agraph_SRCDIR}/scan.c
)
