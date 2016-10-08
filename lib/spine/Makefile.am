# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

#pdfdir = $(pkgdatadir)/doc/pdf
#pkgconfigdir = $(libdir)/pkgconfig

AM_CPPFLAGS = \
    -I$(top_srcdir) \
	-I$(top_srcdir)/lib/cgraph \
	-I$(top_srcdir)/lib/cdt

noinst_HEADERS = spinehdr.h quad.h spine.h subset.h union_find.h
noinst_LTLIBRARIES = libspine_C.la

libspine_C_la_SOURCES = quad.c spine.c subset.c union_find.c

EXTRA_DIST = $(man_MANS) $(pdf_DATA)

DISTCLEANFILES = $(pdf_DATA)

