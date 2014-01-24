# $Id$ $Revision$ 
## Process this file with automake to produce Makefile.in

AGRAPH_VERSION="4:0:0"

pdfdir = $(pkgdatadir)/doc/pdf
pkgconfigdir = $(libdir)/pkgconfig

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/cdt

pkginclude_HEADERS = agraph.h
noinst_HEADERS = aghdr.h malloc.h vmstub.h grammar.h
noinst_LTLIBRARIES = libagraph_C.la
lib_LTLIBRARIES = libagraph.la
pkgconfig_DATA = libagraph.pc
man_MANS = agraph.3
pdf_DATA = agraph.3.pdf

libagraph_C_la_SOURCES = agerror.c apply.c attr.c \
	edge.c flatten.c grammar.y graph.c id.c imap.c io.c \
	mem.c node.c obj.c pend.c rec.c refstr.c scan.l subg.c \
	utils.c write.c

libagraph_la_LDFLAGS = -version-info $(AGRAPH_VERSION) -no-undefined
libagraph_la_SOURCES = $(libagraph_C_la_SOURCES)
libagraph_la_LIBADD = $(top_builddir)/lib/cdt/libcdt.la

scan.o scan.lo: scan.c grammar.h

scan.c: $(top_srcdir)/lib/agraph/scan.l
	@LEX@ -i $(top_srcdir)/lib/agraph/scan.l
	@SED@ "s/yy/aag/g" < @LEX_OUTPUT_ROOT@.c > scan.c
	rm @LEX_OUTPUT_ROOT@.c

grammar.c: y.tab.c
	@SED@ "s/yy/aag/g" < y.tab.c > grammar.c

grammar.h: y.tab.h
	@SED@ "s/yy/aag/g" < y.tab.h > grammar.h

y.tab.c y.tab.h: y.output

y.output: $(top_srcdir)/lib/agraph/grammar.y
	@YACC@ -dv $(top_srcdir)/lib/agraph/grammar.y

agraph.3.pdf: $(srcdir)/agraph.3
	- @GROFF@ -Tps -man $(srcdir)/agraph.3 | @PS2PDF@ - - >agraph.3.pdf

EXTRA_DIST = $(man_MANS) $(pdf_DATA) cmpnd.c dotdge.c \
	main.c tester.c README grammar.c grammar.h scan.c \
	y.tab.c y.tab.h y.output

DISTCLEANFILES = $(pdf_DATA) grammar.[ch] scan.c y.output y.tab.[ch]
