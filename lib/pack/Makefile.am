# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

pdfdir = $(pkgdatadir)/doc/pdf

AM_CPPFLAGS = -I$(top_srcdir) \
        -I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/neatogen \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt

pkginclude_HEADERS = pack.h
noinst_LTLIBRARIES = libpack_C.la
pkglib_LTLIBRARIES = libpack.la
man_MANS = pack.3
pdf_DATA = pack.pdf

libpack_C_la_SOURCES = ccomps.c pack.c

libpack_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined
libpack_la_SOURCES = $(libpack_C_la_SOURCES)
libpack_la_LIBADD = \
	$(top_builddir)/lib/graph/libgraph.la \
	$(top_builddir)/lib/gvc/libgvc.la

.3.pdf:
	groff -Tps -man $< | ps2pdf - - >$@

EXTRA_DIST = $(man_MANS) $(pdf_DATA) Makefile.old ptest.c

DISTCLEANFILES = $(pdf_DATA)
