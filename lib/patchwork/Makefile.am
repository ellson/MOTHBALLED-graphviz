# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

if WITH_CGRAPH
GRAPH = cgraph
NEATOGEN = neatogen_cgraph
FDPGEN = fdpgen_cgraph
else
GRAPH = graph
NEATOGEN = neatogen
FDPGEN = fdpgen
endif

AM_CPPFLAGS = \
        -I$(top_srcdir) \
        -I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/$(NEATOGEN) \
	-I$(top_srcdir)/lib/$(FDPGEN) \
	-I$(top_srcdir)/lib/pack \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/$(GRAPH) \
	-I$(top_srcdir)/lib/cdt

if WITH_CGRAPH
else
noinst_HEADERS = patchwork.h
noinst_LTLIBRARIES = libpatchwork_C.la
endif

libpatchwork_C_la_SOURCES = patchwork.c patchworkinit.c

EXTRA_DIST = Makefile.old
