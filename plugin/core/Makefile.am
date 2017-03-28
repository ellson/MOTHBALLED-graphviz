# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/cgraph \
	-I$(top_srcdir)/lib/cdt \
	-I$(top_srcdir)/lib/gvpr \
	-I$(top_srcdir)/lib/expr \
	-I$(top_srcdir)/lib/ast \
	-I$(top_srcdir)/lib/sfio \
	-I$(top_srcdir)/lib/xdot \
	-I$(top_srcdir)/lib/vmalloc \
	$(LIBGVC_CFLAGS)

noinst_LTLIBRARIES = libgvplugin_core_C.la
if WITH_WIN32
lib_LTLIBRARIES = libgvplugin_core.la
else
pkglib_LTLIBRARIES = libgvplugin_core.la
endif

libgvplugin_core_C_la_SOURCES = \
	gvplugin_core.c \
	gvrender_core_dot.c \
	gvrender_core_json.c \
	gvrender_core_fig.c \
	gvrender_core_map.c \
	gvrender_core_mp.c \
	gvrender_core_ps.c \
	gvrender_core_svg.c \
	gvrender_core_tk.c \
	gvrender_core_vml.c \
	gvrender_core_pov.c \
	gvrender_core_pic.c \
	gvloadimage_core.c \
	ps.txt

libgvplugin_core_la_LDFLAGS = -version-info @GVPLUGIN_VERSION_INFO@
libgvplugin_core_la_SOURCES = $(libgvplugin_core_C_la_SOURCES)
libgvplugin_core_la_LIBADD = $(top_builddir)/lib/gvc/libgvc.la \
				$(top_builddir)/lib/cgraph/libcgraph.la \
				$(top_builddir)/lib/cdt/libcdt.la

gvrender_core_ps.o gvrender_core_ps.lo: ps.h

ps.h : $(srcdir)/ps.txt
	$(AWK) -f $(top_srcdir)/awk/stringize.awk $(srcdir)/ps.txt > ps.h

if WITH_WIN32
libgvplugin_core_la_LDFLAGS += -no-undefined
libgvplugin_core_la_LIBADD += $(top_builddir)/lib/cgraph/libcgraph.la
endif

if WITH_DARWIN9
libgvplugin_core_la_LDFLAGS += -Wl,-exported_symbol,_gvplugin_core_LTX_library
endif

EXTRA_DIST = ps.h gvplugin_core.vcxproj*

DISTCLEANFILES = ps.h
