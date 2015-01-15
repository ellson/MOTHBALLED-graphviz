# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

SFDP_VERSION="1:0:0"

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
	
pkgextraincludedir = $(pkgincludedir)
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libsfdp.pc

noinst_HEADERS = sfdpinternal.h spring_electrical.h \
	sparse_solve.h post_process.h \
	stress_model.h uniform_stress.h \
	Multilevel.h sfdp.h PriorityQueue.h 

pkginclude_HEADERS = sfdp.h spring_electrical.h \
	stress_model.h uniform_stress.h \
	$(top_srcdir)/lib/sparse/SparseMatrix.h $(top_srcdir)/lib/sparse/general.h
	

if WITH_SFDP
lib_LTLIBRARIES = libsfdp.la
noinst_LTLIBRARIES = libsfdpgen_C.la
endif

libsfdpgen_C_la_SOURCES = sfdpinit.c spring_electrical.c \
	sparse_solve.c post_process.c \
	stress_model.c uniform_stress.c \
	QuadTree.c Multilevel.c PriorityQueue.c
	
libsfdp_la_LDFLAGS = -version-info $(SFDP_VERSION) -no-undefined
libsfdp_la_SOURCES = $(libsfdpgen_C_la_SOURCES)
libsfdp_la_LIBADD = $(libsfdpgen_C_la_LIBADD) \
	$(top_builddir)/lib/gvc/libgvc.la \
	$(top_builddir)/lib/cgraph/libcgraph.la \
	$(top_builddir)/lib/cdt/libcdt.la \
	$(top_builddir)/lib/pathplan/libpathplan.la \
	$(top_builddir)/lib/neatogen/libneatogen_C.la \
	$(top_builddir)/lib/sparse/libsparse_C.la \
	$(top_builddir)/lib/rbtree/librbtree_C.la \
	$(GTS_LIBS) $(IPSEPCOLA_LIBS) $(MATH_LIBS)

EXTRA_DIST = sfdp.vcproj
