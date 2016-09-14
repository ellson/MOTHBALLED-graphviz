# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

pdfdir = $(pkgdatadir)/doc/pdf

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/sparse \
	-I$(top_srcdir)/lib/sfdpgen \
	-I$(top_srcdir)/lib/edgepaint \
	-I$(top_srcdir)/lib/ingraphs \
	-I$(top_srcdir)/lib/cgraph \
	-I$(top_srcdir)/lib/cdt

bin_PROGRAMS = edgepaint

man_MANS = edgepaint.1
pdf_DATA = edgepaint.1.pdf

edgepaint_SOURCES = edgepaintmain.c
edgepaint_CPPFLAGS = $(AM_CPPFLAGS)
edgepaint_LDADD = \
    $(top_builddir)/lib/edgepaint/libedgepaint_C.la \
    $(top_builddir)/lib/ingraphs/libingraphs_C.la \
	$(top_builddir)/lib/sfdpgen/libsfdpgen_C.la \
	$(top_builddir)/lib/neatogen/libneatogen_C.la \
	$(top_builddir)/lib/sparse/libsparse_C.la \
	$(top_builddir)/lib/rbtree/librbtree_C.la \
	$(top_builddir)/lib/common/libcommon_C.la \
	$(top_builddir)/lib/edgepaint/liblab_gamut.la \
	$(top_builddir)/lib/cgraph/libcgraph.la \
	$(top_builddir)/lib/cdt/libcdt.la \
	$(ANN_LIBS) -lstdc++ -lm

edgepaint.1.pdf: $(srcdir)/edgepaint.1
	- @GROFF@ -e -Tps -man -t $(srcdir)/edgepaint.1 | @PS2PDF@ - - >edgepaint.1.pdf

EXTRA_DIST = $(man_MANS) $(pdf_DATA) edgepaint.vcxproj*

DISTCLEANFILES = $(pdf_DATA)
