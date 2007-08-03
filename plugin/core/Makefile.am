# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt \
	$(LIBGVC_CFLAGS)

noinst_LTLIBRARIES = libgvplugin_core_C.la

pkglib_LTLIBRARIES = libgvplugin_core.la

libgvplugin_core_C_la_SOURCES = \
	gvplugin_core.c \
	gvrender_core.c \
	gvrender_core_dia.c \
	gvrender_core_dot.c \
	gvrender_core_fig.c \
	gvrender_core_hpgl.c \
	gvrender_core_map.c \
	gvrender_core_mif.c \
	gvrender_core_mp.c \
	gvrender_core_pic.c \
	gvrender_core_ps.c \
	gvrender_core_svg.c \
	gvrender_core_vml.c \
	gvrender_core_vtx.c \
	gvloadimage_core.c

libgvplugin_core_la_LDFLAGS = -version-info @VERSION_INFO@ --no-undefined
libgvplugin_core_la_SOURCES = $(libgvplugin_core_C_la_SOURCES)
libgvplugin_core_la_LIBADD = 

EXTRA_DIST = Makefile.old

