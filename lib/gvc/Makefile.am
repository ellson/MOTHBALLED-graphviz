## Process this file with automake to produce Makefile.in

pkgconfigdir = $(pkglibdir)/pkgconfig

# FIXME - need to get cairo dependencies out of gvemit.c so that @CAIRO...@ can go from here

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt @CAIRO_CFLAGS@

#@INCLTDL@

#LIBS = @LIBLTDL@

pkginclude_HEADERS = gvc.h gvcint.h gvplugin_render.h \
	gvplugin_layout.h gvplugin.h
pkglib_LTLIBRARIES = libgvc.la
pkgconfig_DATA = libgvc.pc

libgvc_la_SOURCES = gvrender.c gvlayout.c gvcontext.c gvjobs.c \
	gvemit.c gvplugin.c gvconfig.c

EXTRA_DIST = Makefile.old
