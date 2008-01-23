# $Id$ $Revision$
# ## Process this file with automake to produce Makefile.in
#
AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/utilities \
	-I$(top_srcdir)/lib/gui \
	-I$(top_srcdir)/cmd/smyrna \
	-I$(top_srcdir)/lib/xdot \
	-I$(top_srcdir)/lib/cgraph \
	-I$(top_srcdir)/lib/cdt \
	$(GTK_CFLAGS) $(GTKGL_CFLAGS) $(GTKGLEXT_CFLAGS) $(GLADE_CFLAGS) $(FREETYPE2_CFLAGS)

if WITH_SMYRNA
noinst_HEADERS = \
	btree.h filter.h 

noinst_LTLIBRARIES = libfilter_C.la
endif

libfilter_C_la_SOURCES = \
	Filter.c btree.c

