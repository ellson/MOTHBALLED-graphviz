# $Id$ $Revision$
# ## Process this file with automake to produce Makefile.in
#

gladedir = $(datadir)/gtktopview/glade

AM_CPPFLAGS = \
	-DGTKTOPVIEW_GLADE=\""$(gladedir)/gtktopview.glade"\" \
	-DGTKTOPVIEW_ATTRS=\""$(gladedir)/attrs.txt"\" \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/xdot \
	-I$(top_srcdir)/lib/cgraph \
	-I$(top_srcdir)/lib/cdt \
	-I$(top_srcdir)/lib/utilities \
	-I$(top_srcdir)/cmd/gtktopview \
	$(GTK_CFLAGS) $(GTKGL_CFLAGS) $(GTKGLEXT_CFLAGS) $(GLADE_CFLAGS) $(FREETYPE2_CFLAGS)

noinst_HEADERS = callbacks.h datalistcallbacks.h gui.h \
	menucallbacks.h toolboxcallbacks.h
noinst_LTLIBRARIES = libGUI_C.la

libGUI_C_la_SOURCES = gui.c callbacks.c datalistcallbacks.c \
	menucallbacks.c toolboxcallbacks.c
