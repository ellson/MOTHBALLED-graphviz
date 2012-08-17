# $Id$Revision: 
## Process this file with automake to produce Makefile.in

if WITH_CGRAPH
GRAPH = cgraph
else
GRAPH = graph
endif

AM_CPPFLAGS = \
        -I$(top_srcdir) \
        -I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/neatogen \
	-I$(top_srcdir)/lib/pack \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/$(GRAPH) \
	-I$(top_srcdir)/lib/cdt

AM_CFLAGS = -D_BLD_gvc=1

noinst_HEADERS = ns.h
noinst_LTLIBRARIES = libns_C.la

libns_C_la_SOURCES = ns.c

EXTRA_DIST = 
 
