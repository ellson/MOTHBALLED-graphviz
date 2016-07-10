# $Id$Revision: 
## Process this file with automake to produce Makefile.in

LAB_GAMUT_VERSION="1:0:0"

pdfdir = $(pkgdatadir)/doc/pdf
pkgconfigdir = $(libdir)/pkgconfig

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/sparse \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/sfdpgen \
	-I$(top_srcdir)/lib/cgraph \
	-I$(top_srcdir)/lib/cdt $(ANN_CFLAGS)

noinst_HEADERS =  edge_distinct_coloring.h  furtherest_point.h  intersection.h  lab_gamut.h  lab.h  node_distinct_coloring.h

noinst_LTLIBRARIES = libedgepaint_C.la
lib_LTLIBRARIES = liblab_gamut.la
pkgconfig_DATA = liblab_gamut.pc
man_MANS = lab_gamut.3
pdf_DATA = lab_gamut.3.pdf

libedgepaint_C_la_SOURCES = edge_distinct_coloring.c  furtherest_point.c  intersection.c  lab.c  node_distinct_coloring.c

liblab_gamut_la_LDFLAGS = -version-info $(LAB_GAMUT_VERSION)
liblab_gamut_la_SOURCES = lab_gamut.c

lab_gamut.3.pdf: $(srcdir)/lab_gamut.3
	- @GROFF@ -Tps -man $(srcdir)/lab_gamut.3 | @PS2PDF@ - - >lab_gamut.3.pdf

EXTRA_DIST =  $(man_MANS) $(pdf_DATA) edgepaintlib.vcxproj* lab_gamut.vcxproj* lab_gamut.def
 
DISTCLEANFILES = $(pdf_DATA)
