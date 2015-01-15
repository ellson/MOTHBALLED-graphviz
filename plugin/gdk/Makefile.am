# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/cgraph \
	-I$(top_srcdir)/lib/cdt \
	$(PANGOCAIRO_CFLAGS) $(GDK_PIXBUF_CFLAGS) $(GDK_CFLAGS)

if WITH_PANGOCAIRO
if WITH_GDK
if WITH_GDK_PIXBUF
noinst_LTLIBRARIES = libgvplugin_gdk_C.la
if WITH_WIN32
lib_LTLIBRARIES = libgvplugin_gdk.la
else
pkglib_LTLIBRARIES = libgvplugin_gdk.la
endif
endif
endif
endif

libgvplugin_gdk_C_la_SOURCES = \
	gvplugin_gdk.c \
	gvdevice_gdk.c \
	gvloadimage_gdk.c

libgvplugin_gdk_la_LDFLAGS = -version-info @GVPLUGIN_VERSION_INFO@
libgvplugin_gdk_la_SOURCES = $(libgvplugin_gdk_C_la_SOURCES)
libgvplugin_gdk_la_LIBADD = \
	$(top_builddir)/lib/gvc/libgvc.la @GDK_PIXBUF_LIBS@ @PANGOCAIRO_LIBS@ @GDK_LIBS@
	
if WITH_WIN32
libgvplugin_gdk_la_LDFLAGS += -no-undefined
endif

if WITH_DARWIN9
libgvplugin_gdk_la_LDFLAGS += -Wl,-exported_symbol,_gvplugin_gdk_LTX_library
endif
