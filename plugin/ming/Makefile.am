# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt \
	$(LIBMING_CFLAGS)

if WITH_LIBMING
noinst_LTLIBRARIES = libgvplugin_ming_C.la
pkglib_LTLIBRARIES = libgvplugin_ming.la
endif

libgvplugin_ming_C_la_SOURCES = \
	gvplugin_ming.c \
	gvrender_ming.c

libgvplugin_ming_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined
libgvplugin_ming_la_SOURCES = $(libgvplugin_ming_C_la_SOURCES)
libgvplugin_ming_la_LIBADD = @LIBMING_LIBS@ @MATH_LIBS@

EXTRA_DIST = Makefile.old

