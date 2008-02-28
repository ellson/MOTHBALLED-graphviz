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

nodist_libgvplugin_gdiplus_C_la_SOURCES = \
	GdiPlusEnums.h \
	GdiPlusHeaders.h \
	GdiPlusImaging.h \
	GdiPlusimageAttributes.h
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

# these targets are built on the build platform, not on the dist platform,
#    because they are included in the nodist...SOURCES list above
GdiPlusEnums.h : $(GDIPLUS_PLATFORMSDK)/include/GdiPlusEnums.h GdiPlusEnums.h.patch
	cp $(GDIPLUS_PLATFORMSDK)/include/GdiPlusEnums.h .
	patch -p0 < GdiPlusEnums.h.patch

GdiPlusHeaders.h : $(GDIPLUS_PLATFORMSDK)/include/GdiPlusHeaders.h GdiPlusHeaders.h.patch
	cp $(GDIPLUS_PLATFORMSDK)/include/GdiPlusHeaders.h .
	patch -p0 < GdiPlusHeaders.h.patch

GdiPlusImaging.h : $(GDIPLUS_PLATFORMSDK)/include/GdiPlusImaging.h GdiPlusImaging.h.patch
	cp $(GDIPLUS_PLATFORMSDK)/include/GdiPlusImaging.h .
	patch -p0 < GdiPlusImaging.h.patch

GdiPlusimageAttributes.h : $(GDIPLUS_PLATFORMSDK)/include/GdiPlusimageAttributes.h GdiPlusimageAttributes.h.patch
	cp $(GDIPLUS_PLATFORMSDK)/include/GdiPlusimageAttributes.h .
	patch -p0 < GdiPlusimageAttributes.h.patch
