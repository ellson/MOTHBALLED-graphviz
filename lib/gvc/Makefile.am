## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt $(INCLTDL)

LIBS = $(LIBLTDL)

pkginclude_HEADERS = gvc.h gvcint.h gvplugin.h \
	gvplugin_render.h gvplugin_layout.h gvplugin_text.h 
pkglib_LTLIBRARIES = libgvc.la

libgvc_la_SOURCES = gvrender.c gvlayout.c gvtext.c gvcontext.c \
	gvjobs.c gvevent.c gvplugin.c gvconfig.c

EXTRA_DIST = Makefile.old
