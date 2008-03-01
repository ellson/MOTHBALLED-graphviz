# $Id$ $Revision$
# ## Process this file with automake to produce Makefile.in
#
AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/cmd/smyrna \
	-I$(top_srcdir)/lib/utilities \
	-I$(top_srcdir)/lib/xdot \
	-I$(top_srcdir)/lib/glcomp \
	-I$(top_srcdir)/lib/cgraph \
	-I$(top_srcdir)/lib/cdt \
	$(GTK_CFLAGS) $(GTKGL_CFLAGS) $(GTKGLEXT_CFLAGS) $(GLADE_CFLAGS) $(FREETYPE2_CFLAGS)

if WITH_SMYRNA
noinst_HEADERS = \
	abstring.h cclass.h cname.h glTexFont.h glTexFontDefs.h \
	glTexFontInclude.h glTexFontTGA.h opengl_pango.h utils.h

noinst_LTLIBRARIES = libutilities_C.la
endif

libutilities_C_la_SOURCES = \
	abstring.c glTexFont.c glTexFontColor.c glTexFontTGA.c \
	opengl_pango.c pangotest.c intersect.c
