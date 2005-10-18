## Process this file with automake to produce Makefile.in

pdfdir = $(pkgdatadir)/doc/pdf
pkgconfigdir = $(libdir)/pkgconfig

pdf = agraph.pdf
man = agraph.3

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/cdt

pkginclude_HEADERS = aghdr.h agraph.h malloc.h vmstub.h grammar.h
pkglib_LTLIBRARIES = libagraph.la
pkgconfig_DATA = libagraph.pc
man_MANS = $(man)
pdf_DATA = $(pdf)

libagraph_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined

libagraph_la_SOURCES = agerror.c apply.c attr.c \
	edge.c flatten.c grammar.y graph.c id.c imap.c io.c \
	mem.c node.c obj.c pend.c rec.c refstr.c scan.l subg.c \
	utils.c write.c

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

.3.pdf:
	groff -Tps -man $< | ps2pdf - - >$@

EXTRA_DIST = $(man) $(pdf) Makefile.old cmpnd.c dotdge.c \
	main.c tester.c README grammar.c grammar.h scan.c y.output

DISTCLEANFILES = $(pdf) grammar.[ch] scan.c y.output y.tab.[ch]
