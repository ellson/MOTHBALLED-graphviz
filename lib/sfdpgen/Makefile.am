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

noinst_HEADERS = SparseMatrix.h sfdpinternal.h spring_electrical.h \
	LinkedList.h overlap.h red_black_tree.h sparse_solve.h post_process.h \
	QuadTree.h call_tri.h Multilevel.h sfdp.h

if WITH_SFDP
noinst_LTLIBRARIES = libsfdpgen_C.la
endif

libsfdpgen_C_la_SOURCES = SparseMatrix.c sfdpinit.c spring_electrical.c \
	LinkedList.c overlap.c red_black_tree.c sparse_solve.c post_process.c \
	QuadTree.c call_tri.c Multilevel.c 

EXTRA_DIST = Makefile.old
