# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt \
	$(GD_INCLUDES) $(LIBGVC_CFLAGS)

noinst_LTLIBRARIES = libgvplugin_gd_C.la

pkglib_LTLIBRARIES = libgvplugin_gd.la

libgvplugin_gd_C_la_SOURCES = \
	gvplugin_gd.c \
	gvrender_gd.c \
	gvrender_gd_vrml.c \
	gvtextlayout_gd.c \
	gvloadimage_gd.c \
	gvdevice_gd.c

libgvplugin_gd_la_LDFLAGS = -version-info @GVPLUGIN_VERSION_INFO@ --no-undefined
libgvplugin_gd_la_SOURCES = $(libgvplugin_gd_C_la_SOURCES)
if WITH_MYLIBGD
libgvplugin_gd_la_LIBADD = \
	$(top_builddir)/lib/gd/libgvgd_C.la \
	@ICONV_LIBS@ @FONTCONFIG_LIBS@ @FREETYPE2_LIBS@ @JPEG_LIBS@ @PNG_LIBS@ @EXPAT_LIBS@ @Z_LIBS@ @MATH_LIBS@
else
libgvplugin_gd_la_LIBADD = @GD_LIBS@ @MATH_LIBS@
endif

EXTRA_DIST = Makefile.old

