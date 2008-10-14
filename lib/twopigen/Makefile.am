# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
        -I$(top_srcdir) \
        -I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/neatogen \
	-I$(top_srcdir)/lib/pack \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt

if WITH_CGRAPH
else
noinst_HEADERS = circle.h
noinst_LTLIBRARIES = libtwopigen_C.la
endif

libtwopigen_C_la_SOURCES = twopiinit.c circle.c

EXTRA_DIST = Makefile.old
