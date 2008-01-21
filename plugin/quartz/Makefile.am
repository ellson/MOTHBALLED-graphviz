# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt \
	$(QUARTZ_CFLAGS)

if WITH_QUARTZ
noinst_LTLIBRARIES = libgvplugin_quartz_C.la
pkglib_LTLIBRARIES = libgvplugin_quartz.la
endif

libgvplugin_quartz_C_la_SOURCES = \
	gvplugin_quartz.c \
	gvrender_quartz.c

libgvplugin_quartz_la_LDFLAGS = -version-info @GVPLUGIN_VERSION_INFO@
libgvplugin_quartz_la_SOURCES = $(libgvplugin_quartz_C_la_SOURCES)
libgvplugin_quartz_la_LIBADD = @QUARTZ_LIBS@
