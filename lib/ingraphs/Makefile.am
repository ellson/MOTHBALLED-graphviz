## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
        -I$(top_srcdir) \
        -I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt 

noinst_HEADERS = ingraphs.h 
noinst_LTLIBRARIES = libingraphs.la

libingraphs_la_SOURCES = ingraphs.c

libingraphs_la_LIBADD = $(top_builddir)/lib/graph/libgraph.la
