# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt \
	$(PANGOCAIRO_CFLAGS) $(GDK_PIXBUF_CFLAGS)

if WITH_PANGOCAIRO
if WITH_GDK_PIXBUF
noinst_LTLIBRARIES = libgvplugin_gdk_pixbuf_C.la
pkglib_LTLIBRARIES = libgvplugin_gdk_pixbuf.la
endif
endif

libgvplugin_gdk_pixbuf_C_la_SOURCES = \
	gvplugin_gdk_pixbuf.c \
	gvformatter_gdk_pixbuf.c

libgvplugin_gdk_pixbuf_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined
libgvplugin_gdk_pixbuf_la_SOURCES = $(libgvplugin_gdk_pixbuf_C_la_SOURCES)
libgvplugin_gdk_pixbuf_la_LIBADD = @GDK_PIXBUF_LIBS@

EXTRA_DIST = Makefile.old

