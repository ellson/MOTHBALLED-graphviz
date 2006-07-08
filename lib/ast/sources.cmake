# $Id$ $Revision$

INCLUDE_DIRECTORIES(
	${top_SRCDIR}
	${ast_SRCDIR}
	${sfio_SRCDIR}
)

SET(ast_SRCS
	${ast_SRCDIR}/pathpath.c
	${ast_SRCDIR}/sfstr.h
	${ast_SRCDIR}/chresc.c
	${ast_SRCDIR}/chrtoi.c
	${ast_SRCDIR}/error.c
	${ast_SRCDIR}/fmtbuf.c
	${ast_SRCDIR}/fmtesc.c
	${ast_SRCDIR}/pathaccess.c
	${ast_SRCDIR}/pathbin.c
	${ast_SRCDIR}/pathcanon.c
	${ast_SRCDIR}/pathcat.c
	${ast_SRCDIR}/pathexists.c
	${ast_SRCDIR}/pathfind.c
	${ast_SRCDIR}/pathgetlink.c
	${ast_SRCDIR}/strcopy.c
	${ast_SRCDIR}/stresc.c
	${ast_SRCDIR}/strmatch.c
	${ast_SRCDIR}/strton.c
	${ast_SRCDIR}/strerror.c
)
