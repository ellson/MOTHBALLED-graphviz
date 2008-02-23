# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt \
	$(PANGOCAIRO_CFLAGS) $(LIBGVC_CFLAGS)

if WITH_PANGOCAIRO
noinst_LTLIBRARIES = libgvplugin_pango_C.la
if WITH_WIN32
lib_LTLIBRARIES = libgvplugin_pango.la
else
pkglib_LTLIBRARIES = libgvplugin_pango.la
endif
endif

libgvplugin_pango_C_la_SOURCES = \
	gvplugin_pango.c \
	gvrender_pango.c \
	gvtextlayout_pango.c \
	gvloadimage_pango.c

libgvplugin_pango_la_LDFLAGS = -version-info @GVPLUGIN_VERSION_INFO@ -no-undefined
libgvplugin_pango_la_SOURCES = $(libgvplugin_pango_C_la_SOURCES)
libgvplugin_pango_la_LIBADD = \
	$(top_builddir)/lib/gvc/libgvc.la @PANGOCAIRO_LIBS@ @FONTCONFIG_LIBS@ @MATH_LIBS@


EXTRA_DIST = Makefile.old

