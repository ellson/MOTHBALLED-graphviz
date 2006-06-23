# $Id$ $Revision$
## Process this file with cmake to produce Makefile

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/lib/vmalloc
	${CMAKE_SOURCE_DIR}/lib/sfio
	${CMAKE_SOURCE_DIR}/lib/ast
	${CMAKE_SOURCE_DIR}/lib/cdt
)


########### next target ###############

ADD_CUSTOM_COMMAND(
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
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
	exparse.h
	exop.h
	excc.c
	excontext.c
	exdata.c
	exerror.c
	exeval.c
	exexpr.c
	exlexname.c
	exopen.c
	exrewind.c
	extoken.c
	extype.c
	exzero.c
	exparse.c
)

ADD_LIBRARY(expr STATIC ${expr_SRCS})


########### install files ###############

INSTALL_FILES(FILES
	expr.pdf
)
INSTALL_FILES(FILES
	expr.1
)



#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#pdfdir = $(pkgdatadir)/doc/pdf
#
#AM_CPPFLAGS = \
#        -I$(top_srcdir) \
#        -I$(top_srcdir)/lib/vmalloc \
#	-I$(top_srcdir)/lib/sfio \
#	-I$(top_srcdir)/lib/ast \
#	-I$(top_srcdir)/lib/cdt
#
#pkginclude_HEADERS = exgram.h exlib.h expr.h
#noinst_LTLIBRARIES = libexpr_C.la
#pkglib_LTLIBRARIES = libexpr.la
#man_MANS = expr.3
#pdf_DATA = expr.pdf
#
##### -no-undefined breaks Mac OS/X builds.
##### libast has "extern char **environ" which remains
#####      undefined until linked with main()
##libexpr_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined
#
#libexpr_C_la_SOURCES = excc.c excontext.c exdata.c exerror.c \
#	exeval.c exexpr.c exlexname.c exopen.c exrewind.c extoken.c \
#	extype.c exzero.c exparse.y
#libexpr_C_la_LIBADD = \
#	$(top_builddir)/lib/ast/libast_C.la \
#	$(top_builddir)/lib/vmalloc/libvmalloc_C.la \
#	$(top_builddir)/lib/sfio/libsfio_C.la
#
#libexpr_la_LDFLAGS = -version-info @VERSION_INFO@
#libexpr_la_SOURCES = $(libexpr_C_la_SOURCES)
#libexpr_la_LIBADD = $(libexpr_C_la_LIBADD)
#	$(top_builddir)/lib/cdt/libcdt.la
#
#$(libexpr_la_OBJECTS): exparse.h exparse.c exgram.h exop.h
#
#y.output: $(top_srcdir)/lib/expr/exparse.y
#	@YACC@ -dtv $(top_srcdir)/lib/expr/exparse.y
#
#exparse.c: y.output 
#	@SED@ "s/yy/ex/g" <y.tab.c | \
#		@SED@ "s/YY/EX/g" | \
#		@SED@ "s/fprintf/sfprintf/g" | \
#		@SED@ "s/FILE/Sfio_t/g" | \
#		@SED@ "s/stderr/sfstderr/g" > exparse.c
#
#exparse.h: y.output
#	echo "#ifndef _EXPARSE_H" > exparse.h
#	echo "#define _EXPARSE_H" >> exparse.h
#	@SED@ "s/yy/ex/g" < y.tab.h | @SED@ "s/YY/EX/g" >> exparse.h
#	echo "#endif /* _EXPARSE_H */" >> exparse.h
#
#exop.h: exparse.h
#	echo "static const char* exop[] = {" > exop.h
#	echo "	\"MINTOKEN\"," >> exop.h
#	$(SED) -e '1,/MINTOKEN/d' -e '/^[ 	]*#[ 	]*define[ 	][ 	]*[A-Z]/!d' -e 's/^[ 	]*#[ 	]*define[ 	]*\([A-Z0-9_]*\).*/	"\1",/' < exparse.h >> exop.h
#	echo "};" >> exop.h
#
#.3.pdf:
#	groff -Tps -man $< | ps2pdf - - >$@
#
#EXTRA_DIST = $(man_MANS) $(pdf_DATA) RELEASE \
#	Makefile.nmake Makefile.orig Makefile.old \
#	exparse.c exparse.h exop.h \
#	y.tab.c y.tab.h y.output
#
#DISTCLEANFILES = $(pdf_DATA) exparse.[ch] exop.h y.tab.[ch] y.output
