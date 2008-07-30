# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
        -I$(top_srcdir) \
        -I$(top_srcdir)/lib/cdt \
        -I$(top_srcdir)/lib/graph \
        -I$(top_srcdir)/lib/pathplan \
        -I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/common 

noinst_HEADERS = SparseMatrix.h
noinst_LTLIBRARIES = libsparse_C.la

libsparse_C_la_SOURCES = SparseMatrix.c

EXTRA_DIST = Makefile.old
