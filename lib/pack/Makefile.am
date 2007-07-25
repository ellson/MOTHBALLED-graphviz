# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

pdfdir = $(pkgdatadir)/doc/pdf
#pkgconfigdir = $(libdir)/pkgconfig

AM_CPPFLAGS = -I$(top_srcdir) \
        -I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/neatogen \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt

if HAVE_WIN32
AM_CFLAGS = -D_BLD_common=1
endif

pkginclude_HEADERS = pack.h
noinst_LTLIBRARIES = libpack_C.la
#man_MANS = pack.3
#pdf_DATA = pack.pdf
#pkgconfig_DATA = libpack.pc

libpack_C_la_SOURCES = ccomps.c pack.c

#.3.pdf:
#	-groff -Tps -man $< | ps2pdf - - >$@

#EXTRA_DIST = $(man_MANS) $(pdf_DATA) Makefile.old ptest.c

#DISTCLEANFILES = $(pdf_DATA)

EXTRA_DIST = pack.3 Makefile.old ptest.c
