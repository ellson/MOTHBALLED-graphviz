## Process this file with automake to produce Makefile.in

pdfdir = $(pkgdatadir)/doc/pdf

AM_CPPFLAGS = \
        -I$(top_srcdir) \
        -I$(top_srcdir)/tools/src \
	-I$(top_srcdir)/agraph \
	-I$(top_srcdir)/cdt

bin_PROGRAMS = prune
noinst_HEADERS = generic_list.h
man_MANS = prune.1
pdf_DATA = prune.pdf

prune_SOURCES = generic_list.c prune.c

prune_LDADD = $(top_builddir)/tools/src/libingraphs.la \
	$(top_builddir)/agraph/libagraph.la \
	$(top_builddir)/cdt/libcdt.la

.1.pdf:
	groff -Tps -man $< | ps2pdf - - >$@

EXTRA_DIST = $(man_MANS) $(pdf_DATA) Makefile.old Makefile.orig README test.dot

DISTCLEANFILES = $(pdf_DATA)
