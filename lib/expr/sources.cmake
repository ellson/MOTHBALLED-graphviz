# $Id$ $Revision$

INCLUDE_DIRECTORIES(
	${expr_SRCDIR}
	${top_SRCDIR}
	${vmalloc_SRCDIR}
	${sfio_SRCDIR}
	${ast_SRCDIR}
	${cdt_SRCDIR}
)

ADD_CUSTOM_COMMAND(
	WORKING_DIRECTORY ${expr_SRCDIR}
	OUTPUT exparse.c
	OUTPUT exparse.h
	OUTPUT exop.h
	DEPENDS exparse.y
	COMMAND yacc -dv exparse.y
	COMMAND sed 's/yy/ex/g' < y.tab.c | sed 's/YY/EX/g' | sed 's/fprintf/sfprintf/g' | sed 's/FILE/Sfio_t/g' | sed 's/stderr/sfstderr/g' > exparse.c
	COMMAND echo "\#ifndef _EXPARSE_H" > exparse.h
	COMMAND echo "\#define _EXPARSE_H" >> exparse.h
	COMMAND sed "s/yy/ex/g" < y.tab.h | sed "s/YY/EX/g" >> exparse.h
	COMMAND echo "\#endif /* _EXPARSE_H */" >> exparse.h
	COMMAND echo "static const char* exop[] = {" > exop.h
	COMMAND echo "	\"MINTOKEN\"," >> exop.h
	COMMAND sed -e "1,/MINTOKEN/d" -e "/^[ 	]*\#[ 	]*define[ 	][ ]*[A-Z]/!d" -e "s/^[ 	]*\#[ 	]*define[ 	]*\\\([A-Z0-9_]*\\\).*/ \"\\1\",/" < exparse.h >> exop.h
	COMMAND echo "};" >> exop.h
	COMMAND rm y.tab.c y.tab.h y.output
)

SET(expr_SRCS
	${expr_SRCDIR}/exparse.h
	${expr_SRCDIR}/exop.h
	${expr_SRCDIR}/excc.c
	${expr_SRCDIR}/excontext.c
	${expr_SRCDIR}/exdata.c
	${expr_SRCDIR}/exerror.c
	${expr_SRCDIR}/exeval.c
	${expr_SRCDIR}/exexpr.c
	${expr_SRCDIR}/exlexname.c
	${expr_SRCDIR}/exopen.c
	${expr_SRCDIR}/exrewind.c
	${expr_SRCDIR}/extoken.c
	${expr_SRCDIR}/extype.c
	${expr_SRCDIR}/exzero.c
	${expr_SRCDIR}/exparse.c
)
