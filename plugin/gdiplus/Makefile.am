# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt
	
if WITH_GDIPLUS
noinst_LTLIBRARIES = libgvplugin_gdiplus_C.la
if WITH_WIN32
lib_LTLIBRARIES = libgvplugin_gdiplus.la
else
pkglib_LTLIBRARIES = libgvplugin_gdiplus.la
endif
endif

libgvplugin_gdiplus_C_la_SOURCES = \
	gvplugin_gdiplus.cpp \
	gvrender_gdiplus.cpp

libgvplugin_gdiplus_la_LDFLAGS = -version-info @GVPLUGIN_VERSION_INFO -L "$(GDIPLUS_PLATFORMSDK)/lib"
nodist_libgvplugin_gdiplus_la_SOURCES = $(nodist_libgvplugin_gdiplus_C_la_SOURCES)
libgvplugin_gdiplus_la_SOURCES = $(libgvplugin_gdiplus_C_la_SOURCES)
libgvplugin_gdiplus_la_LIBADD = -lgdiplus -lgdi32 -lole32 -lstdc++

if WITH_WIN32
libgvplugin_gdiplus_la_LDFLAGS += -no-undefined
endif

libgvplugin_gdiplus.la libgvplugin_gdiplus_C.la: GdiPlusHeaders

# these targets are created on the build platform, not on the dist platform
.PHONY: GdiPlusHeaders
GdiPlusHeaders: gdiplus.diff
	cp $(GDIPLUS_PLATFORMSDK)/include/*.h .
	patch -p0 < gdiplus.diff

EXTRA_DIST = gdiplus.diff
