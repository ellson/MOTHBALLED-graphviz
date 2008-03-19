# $Id$ $Revision$
# ## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	$(GTK_CFLAGS) $(GTKGL_CFLAGS) $(GTKGLEXT_CFLAGS) $(GLADE_CFLAGS) $(FREETYPE2_CFLAGS)

if WITH_SMYRNA
noinst_HEADERS = glcompset.h glcomptexture.h glutils.h \
	glTexFont.h glTexFontDefs.h glTexFontInclude.h glTexFontTGA.h
noinst_LTLIBRARIES = libglcomp_C.la
endif

libglcomp_C_la_SOURCES = glcompset.c glcomptexture.c glutils.c \
	glTexFont.c glTexFontColor.c glTexFontTGA.c
