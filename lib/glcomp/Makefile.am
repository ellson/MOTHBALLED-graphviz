# $Id$ $Revision$
# ## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/cgraph \
	-I$(top_srcdir)/lib/cdt \
	-I$(top_srcdir)/lib/utilities \
	-I$(top_srcdir)/cmd/smyrna \
	$(GTK_CFLAGS) $(GTKGL_CFLAGS) $(GTKGLEXT_CFLAGS) $(GLADE_CFLAGS) $(FREETYPE2_CFLAGS)

if WITH_SMYRNA
noinst_HEADERS = glcompset.h glcomptexture.h glutils.h
noinst_LTLIBRARIES = libglcomp_C.la
endif

libglcomp_C_la_SOURCES = glcompset.c glcomptexture.c glutils.c
