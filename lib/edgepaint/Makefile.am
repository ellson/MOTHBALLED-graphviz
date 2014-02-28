# $Id$Revision: 
## Process this file with automake to produce Makefile.in

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

libedgepaint_C_la_SOURCES = edge_distinct_coloring.c  furtherest_point.c  intersection.c  lab.c  node_distinct_coloring.c

EXTRA_DIST = edgepaintlib.vcproj
 
