# $Id$ $Revision$
# ## Process this file with automake to produce Makefile.in
#
AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/cgraph \
	-I$(top_srcdir)/lib/cdt \
	-I$(top_srcdir)/lib/common \
	$(GTK_CFLAGS) $(GTKGL_CFLAGS) $(GTKGLEXT_CFLAGS) $(GLADE_CFLAGS) $(FREETYPE2_CFLAGS)

if WITH_SMYRNA
noinst_HEADERS = matrix_ops.h triangle.h defs.h hierarchy.h
noinst_LTLIBRARIES = libtopfish_C.la
endif

libtopfish_C_la_SOURCES = delaunay.c hierarchy.c matrix_ops.c \
			  rescale_layout.c triangle.c
