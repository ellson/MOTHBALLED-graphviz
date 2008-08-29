# $Id$ $Revision$ 
## Process this file with automake to produce Makefile.in

pdfdir = $(pkgdatadir)/doc/pdf
pkgconfigdir = $(libdir)/pkgconfig

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/cdt

if WITH_CGRAPH
else
pkginclude_HEADERS = agraph.h
noinst_HEADERS = aghdr.h malloc.h vmstub.h grammar.h
noinst_LTLIBRARIES = libagraph_C.la
lib_LTLIBRARIES = libagraph.la
pkgconfig_DATA = libagraph.pc
man_MANS = agraph.3
pdf_DATA = agraph.3.pdf
endif

libagraph_C_la_SOURCES = agerror.c apply.c attr.c \
	edge.c flatten.c grammar.y graph.c id.c imap.c io.c \
	mem.c node.c obj.c pend.c rec.c refstr.c scan.l subg.c \
	utils.c write.c

libagraph_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined
libagraph_la_SOURCES = $(libagraph_C_la_SOURCES)
libagraph_la_LIBADD = $(top_builddir)/lib/cdt/libcdt.la

scan.o scan.lo: scan.c grammar.h

scan.c: $(top_srcdir)/lib/agraph/scan.l
	@LEX@ -i $(top_srcdir)/lib/agraph/scan.l
	@SED@ "s/yy/aag/g" < @LEX_OUTPUT_ROOT@.c > scan.c
	rm @LEX_OUTPUT_ROOT@.c

y.output: $(top_srcdir)/lib/agraph/grammar.y
	@YACC@ -dv $(top_srcdir)/lib/agraph/grammar.y

grammar.c: y.output
	@SED@ "s/yy/aag/g" < y.tab.c > grammar.c

grammar.h: y.output
	@SED@ "s/yy/aag/g" < y.tab.h > grammar.h

agraph.3.pdf: agraph.3
	- @GROFF@ -Tps -man agraph.3 | @PS2PDF@ - - >agraph.3.pdf

EXTRA_DIST = $(man_MANS) $(pdf_DATA) Makefile.old cmpnd.c dotdge.c \
	main.c tester.c README grammar.c grammar.h scan.c \
	y.tab.c y.tab.h y.output

DISTCLEANFILES = $(pdf_DATA) grammar.[ch] scan.c y.output y.tab.[ch]
