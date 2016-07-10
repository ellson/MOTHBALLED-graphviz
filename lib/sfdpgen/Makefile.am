# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
        -I$(top_srcdir) \
        -I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/neatogen \
	-I$(top_srcdir)/lib/sparse \
	-I$(top_srcdir)/lib/rbtree \
	-I$(top_srcdir)/lib/pack \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/cgraph \
	-I$(top_srcdir)/lib/cdt

noinst_HEADERS = sfdpinternal.h spring_electrical.h \
	sparse_solve.h post_process.h \
	stress_model.h uniform_stress.h \
	Multilevel.h sfdp.h PriorityQueue.h 

if WITH_SFDP
noinst_LTLIBRARIES = libsfdpgen_C.la
endif

libsfdpgen_C_la_SOURCES = sfdpinit.c spring_electrical.c \
	sparse_solve.c post_process.c \
	stress_model.c uniform_stress.c \
	Multilevel.c PriorityQueue.c

EXTRA_DIST = sfdp.vcxproj*
