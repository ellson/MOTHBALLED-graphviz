# $Id$ $Revision$
# ## Process this file with automake to produce Makefile.in
#

guidir = $(pkgdatadir)/gui

AM_CPPFLAGS = \
	-DSMYRNA_GLADE=\""$(guidir)/smyrna.glade"\" \
	-DSMYRNA_ATTRS=\""$(guidir)/attrs.txt"\" \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/xdot \
	-I$(top_srcdir)/lib/cgraph \
	-I$(top_srcdir)/lib/cdt \
	-I$(top_srcdir)/lib/utilities \
	-I$(top_srcdir)/cmd/smyrna \
	$(GTK_CFLAGS) $(GTKGL_CFLAGS) $(GTKGLEXT_CFLAGS) $(GLADE_CFLAGS) $(FREETYPE2_CFLAGS)

if WITH_SMYRNA
noinst_HEADERS = callbacks.h datalistcallbacks.h gui.h \
	menucallbacks.h toolboxcallbacks.h
noinst_LTLIBRARIES = libgui_C.la
endif

libgui_C_la_SOURCES = gui.c callbacks.c datalistcallbacks.c \
	menucallbacks.c toolboxcallbacks.c
