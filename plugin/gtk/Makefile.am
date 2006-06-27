# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	$(GTK_CFLAGS) $(GNOMEUI_CFLAGS) $(LIBGVC_CFLAGS)

if WITH_GTK
lib_LTLIBRARIES = libgvplugin_gtk.la
endif

libgvplugin_gtk_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined

libgvplugin_gtk_la_SOURCES = \
	gvplugin_gtk.c \
	gvdevice_gtk.c \
	support.c support.h \
        interface.c interface.h \
        callbacks.c callbacks.h

libgvplugin_gtk_la_LIBADD = $(GTK_LIBS) $(GNOMEUI_LIBS)

CLEANFILES = *.BAK

EXTRA_DIST = gtk.glade gtk.gladep
