# $Id$ $Revision$

INCLUDE_DIRECTORIES(
	${graph_SRCDIR}
	${top_SRCDIR}
	${cdt_SRCDIR}
)

ADD_CUSTOM_COMMAND(
	WORKING_DIRECTORY ${graph_SRCDIR}
	OUTPUT parser.c
	OUTPUT parser.h
	DEPENDS parser.y
	COMMAND yacc -dv parser.y
	COMMAND sed "s/yy/ag/g" < y.tab.c > parser.c
	COMMAND sed "s/yy/ag/g" < y.tab.h > parser.h
	COMMAND rm y.tab.c y.tab.h y.output
)

SET(graph_SRCS
	${graph_SRCDIR}/attribs.c
	${graph_SRCDIR}/agxbuf.c
	${graph_SRCDIR}/edge.c
	${graph_SRCDIR}/graph.c
	${graph_SRCDIR}/graphio.c
	${graph_SRCDIR}/lexer.c
	${graph_SRCDIR}/node.c
	${graph_SRCDIR}/parser.c
	${graph_SRCDIR}/refstr.c
	${graph_SRCDIR}/trie.c
)
