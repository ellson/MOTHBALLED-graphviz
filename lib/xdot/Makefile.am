# $Id$ $Revision$
# ## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/cgraph \
	-I$(top_srcdir)/lib/cdt \
	-I$(top_srcdir)/lib/utilities \
	-I$(top_srcdir)/cmd/gtktopview \
	$(GTK_CFLAGS) $(GTKGL_CFLAGS) $(GTKGLEXT_CFLAGS) $(GLADE_CFLAGS) $(FREETYPE2_CFLAGS)

noinst_HEADERS = xdot.h
noinst_LTLIBRARIES = libxdot_C.la

libxdot_C_la_SOURCES = xdot.c
