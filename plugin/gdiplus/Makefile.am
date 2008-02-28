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
	gvplugin_gdiplus.c \
	gvrender_gdiplus.c

libgvplugin_gdiplus_la_LDFLAGS = -version-info @GVPLUGIN_VERSION_INFO -L "$(PLATFORMSDK)/lib"
libgvplugin_gdiplus_la_SOURCES = $(libgvplugin_gdiplus_C_la_SOURCES)
libgvplugin_gdiplus_la_LIBADD = -lgdiplus -lgdi32 -lole32 -lstdc++

if WITH_WIN32
libgvplugin_gdiplus_la_LDFLAGS += -no-undefined
endif

Gdiplus.h : $(PLATFORMSDK)/include/Gdiplus*.h gdiplus.diff
	cp $(PLATFORMSDK)/include/Gdiplus*.h .
	patch gdiplus.diff
