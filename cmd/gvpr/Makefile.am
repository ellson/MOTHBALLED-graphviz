## Process this file with automake to produce Makefile.in

pdfdir = $(pkgdatadir)/doc/pdf

AM_CPPFLAGS = \
        -I$(top_srcdir) \
        -I$(top_srcdir)/lib/expr \
	-I$(top_srcdir)/lib/vmalloc \
	-I$(top_srcdir)/lib/sfio \
	-I$(top_srcdir)/lib/ast \
	-I$(top_srcdir)/lib/ingraphs \
	-I$(top_srcdir)/lib/agraph \
	-I$(top_srcdir)/lib/cdt

bin_PROGRAMS = gvpr
noinst_PROGRAMS = mkdefs
noinst_HEADERS = actions.h compile.h gprstate.h parse.h queue.h
man_MANS = gvpr.1
pdf_DATA = gvpr.pdf

gvpr_SOURCES = actions.c compile.c gvpr.c gprstate.c parse.c queue.c

gvpr_LDADD = $(top_builddir)/lib/expr/libexpr.la \
        $(top_builddir)/lib/vmalloc/libvmalloc.la \
	$(top_builddir)/lib/sfio/libsfio.la \
        $(top_builddir)/lib/ingraphs/libingraphs.la \
	$(top_builddir)/lib/agraph/libagraph.la @MATH_LIBS@

gvpr_DEPENDENCIES = \
       $(top_builddir)/lib/ast/libast.la

$(gvpr_OBJECTS): gdefs.h

mkdefs_SOURCES = mkdefs.c

gdefs.h: $(top_srcdir)/cmd/gvpr/gprdata mkdefs
	./mkdefs gdefs.h < $(top_srcdir)/cmd/gvpr/gprdata

.1.pdf:
	groff -Tps -man $< | ps2pdf - - >$@

EXTRA_DIST = $(man_MANS) $(pdf_DATA) Makefile.old gprdata mkdefs.c

CLEANFILES = gdefs.h mkdefs

DISTCLEANFILES = $(pdf_DATA)
