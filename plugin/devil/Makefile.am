# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt \
	$(PANGOCAIRO_CFLAGS) $(DEVIL_CFLAGS)

if WITH_DEVIL
noinst_LTLIBRARIES = libgvplugin_devil_C.la
pkglib_LTLIBRARIES = libgvplugin_devil.la
endif

libgvplugin_devil_C_la_SOURCES = \
	gvplugin_devil.c \
	gvformatter_devil.c

libgvplugin_devil_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined
libgvplugin_devil_la_SOURCES = $(libgvplugin_devil_C_la_SOURCES)
libgvplugin_devil_la_LIBADD = @DEVIL_LIBS@

EXTRA_DIST = Makefile.old

