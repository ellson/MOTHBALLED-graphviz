# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	$(PANGOCAIRO_CFLAGS) \
	$(LIBGVC_CFLAGS)

if WITH_X
lib_LTLIBRARIES = libgvplugin_xlib.la
endif

libgvplugin_xlib_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined

libgvplugin_xlib_la_SOURCES = \
	gvplugin_xlib.c \
	gvdevice_xlib.c

libgvplugin_xlib_la_LIBADD = \
	$(PANGOCAIRO_LIBS)
