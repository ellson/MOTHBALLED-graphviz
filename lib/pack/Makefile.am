## Process this file with automake to produce Makefile.in

pdfdir = $(pkgdatadir)/doc/pdf
pkgconfigdir = $(pkglibdir)/pkgconfig

AM_CPPFLAGS = -I$(top_srcdir) \
        -I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/neatogen \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt

pkginclude_HEADERS = pack.h
pkglib_LTLIBRARIES = libpack.la
pkgconfig_DATA = libpack.pc
man_MANS = pack.3
pdf_DATA = pack.pdf

libpack_la_SOURCES = ccomps.c pack.c

.3.pdf:
	groff -Tps -man $< | ps2pdf - - >$@

EXTRA_DIST = $(man_MANS) $(pdf_DATA) Makefile.old ptest.c

DISTCLEANFILES = $(pdf_DATA)
