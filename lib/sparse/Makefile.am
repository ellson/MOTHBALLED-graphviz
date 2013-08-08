# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
        -I$(top_srcdir) \
	-I$(top_srcdir)/lib/sfdpgen \ 
	-I$(top_srcdir)/lib/common 

noinst_HEADERS = SparseMatrix.h general.h BinaryHeap.h IntStack.h
noinst_LTLIBRARIES = libsparse_C.la

libsparse_C_la_SOURCES = SparseMatrix.c general.c BinaryHeap.c IntStack.c

EXTRA_DIST = Makefile.old gvsparse.vcproj
