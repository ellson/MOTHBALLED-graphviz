# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt
	
PATCHES = GdiPlusEnums.h.patch GdiPlusHeaders.h.patch GdiPlusimageAttributes.h.patch GdiPlusImaging.h.patch
PATCHED_HEADS = $(PATCHES:.h.patch=.h)

if WITH_GDIPLUS
noinst_LTLIBRARIES = libgvplugin_gdiplus_C.la
if WITH_WIN32
lib_LTLIBRARIES = libgvplugin_gdiplus.la
else
pkglib_LTLIBRARIES = libgvplugin_gdiplus.la
endif
endif

nodist_libgvplugin_gdiplus_C_la_SOURCES = GdiPlus*.h
libgvplugin_gdiplus_C_la_SOURCES = \
	gvplugin_gdiplus.cpp \
	gvrender_gdiplus.cpp

# libtool doesn't want to run with actual Windows import libs, so we force GdiPlus.lib through to the linker
libgvplugin_gdiplus_la_LDFLAGS = -version-info @GVPLUGIN_VERSION_INFO@ -Wl,"$(PLATFORMSDKLIB)\GdiPlus.lib"
nodist_libgvplugin_gdiplus_la_SOURCES = GdiPlus*.h
libgvplugin_gdiplus_la_SOURCES = $(libgvplugin_gdiplus_C_la_SOURCES)
libgvplugin_gdiplus_la_LIBADD = -lgdi32 -lole32 -lstdc++ $(top_builddir)/lib/gvc/libgvc.la

if WITH_WIN32
libgvplugin_gdiplus_la_LDFLAGS += -no-undefined
endif

# object code is dependent on all the local GDI+ headers
gvrender_gdiplus.lo : $(GDIPLUS_HEADS)

# patched GDI+ headers are dependent on the patches
$(PATCHED_HEADS) : %.h : %.h.patch
	
# all the local GDI+ headers are dependent on the originals in PlatformSDK
# if the GDI+ header needs to be patched, patch it, else just copy it over
$(GDIPLUS_HEADS) : %.h : $(PLATFORMSDKINCLUDE_ESCAPED)/%.h
	if expr "$(PATCHED_HEADS)" : '.*$*.h' >/dev/null 2>&1; \
	then patch -o $*.h "$(PLATFORMSDKINCLUDE)\$*.h" $*.h.patch; \
	else cp "$(PLATFORMSDKINCLUDE)\$*.h" $*.h; \
	fi
	
EXTRA_DIST = $(PATCHES)
