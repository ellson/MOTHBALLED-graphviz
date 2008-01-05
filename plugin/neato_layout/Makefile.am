# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt \
	$(LIBGVC_CFLAGS)

noinst_LTLIBRARIES = libgvplugin_neato_layout_C.la

pkglib_LTLIBRARIES = libgvplugin_neato_layout.la

libgvplugin_neato_layout_C_la_SOURCES = \
	gvplugin_neato_layout.c \
	gvlayout_neato_layout.c
libgvplugin_neato_layout_C_la_LIBADD = \
	$(top_builddir)/lib/neatogen/libneatogen_C.la \
	$(top_builddir)/lib/twopigen/libtwopigen_C.la \
	$(top_builddir)/lib/fdpgen/libfdpgen_C.la \
	$(top_builddir)/lib/circogen/libcircogen_C.la

libgvplugin_neato_layout_la_LDFLAGS = -version-info @GVPLUGIN_VERSION_INFO@
libgvplugin_neato_layout_la_SOURCES = $(libgvplugin_neato_layout_C_la_SOURCES)
libgvplugin_neato_layout_la_LIBADD = $(libgvplugin_neato_layout_C_la_LIBADD) \
	$(top_builddir)/lib/gvc/libgvc.la \
	$(top_builddir)/lib/pathplan/libpathplan.la @IPSEPCOLA_LIBS@

EXTRA_DIST = Makefile.old
