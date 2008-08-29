# $Id$ $Revision$ 
## Process this file with automake to produce Makefile.in

pdfdir = $(pkgdatadir)/doc/pdf
pkgconfigdir = $(libdir)/pkgconfig

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/cdt

pkginclude_HEADERS = cgraph.h
noinst_HEADERS = agxbuf.h cghdr.h malloc.h vmstub.h
noinst_LTLIBRARIES = libcgraph_C.la
lib_LTLIBRARIES = libcgraph.la
pkgconfig_DATA = libcgraph.pc
man_MANS = cgraph.3
pdf_DATA = cgraph.3.pdf

libcgraph_C_la_SOURCES = agerror.c agxbuf.c apply.c attr.c edge.c \
	flatten.c graph.c grammar.y id.c imap.c io.c mem.c node.c \
	obj.c pend.c rec.c refstr.c scan.l subg.c utils.c write.c

libcgraph_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined
libcgraph_la_SOURCES = $(libcgraph_C_la_SOURCES)
libcgraph_la_LIBADD = $(top_builddir)/lib/cdt/libcdt.la

scan.o scan.lo: scan.c grammar.h

scan.c: $(top_srcdir)/lib/cgraph/scan.l
	@LEX@ -i $(top_srcdir)/lib/cgraph/scan.l
	@SED@ "s/yy/aag/g" < @LEX_OUTPUT_ROOT@.c > scan.c
	rm @LEX_OUTPUT_ROOT@.c

y.output: $(top_srcdir)/lib/cgraph/grammar.y
	@YACC@ -dv $(top_srcdir)/lib/cgraph/grammar.y

grammar.c: y.output
	@SED@ "s/yy/aag/g" < y.tab.c > grammar.c

grammar.h: y.output
	@SED@ "s/yy/aag/g" < y.tab.h > grammar.h

cgraph.3.pdf: cgraph.3
	- @GROFF@ -Tps -man cgraph.3 | @PS2PDF@ - - >cgraph.3.pdf

EXTRA_DIST = $(man_MANS) $(pdf_DATA) Makefile.old cmpnd.c \
	main.c tester.c grammar.c grammar.h scan.c \
	y.tab.c y.tab.h y.output

DISTCLEANFILES = $(pdf_DATA) grammar.[ch] scan.c y.output y.tab.[ch]
