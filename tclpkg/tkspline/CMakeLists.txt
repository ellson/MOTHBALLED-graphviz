# $Id$ $Revision$
## Process this file with cmake to produce Makefile

SUBDIRS( demo ) 

INCLUDE(
	FindTCL
)

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}
	${TK_INCLUDE_PATH}
)

SET(tkspline_SRCS
	tkspline.c
)

ADD_LIBRARY(tkspline MODULE ${tkspline_SRCS})

########### install files ###############

INSTALL_FILES(FILES
	tkspline.pdf
)
INSTALL_FILES(FILES
	tkspline.n
)



#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#SUBDIRS = demo
#
#pkgtcldir = $(pkglibdir)/tcl
#pdfdir = $(pkgdatadir)/doc/pdf
#
#man = tkspline.n
#pdf = tkspline.pdf
#
#AM_CPPFLAGS = \
#	-I@TK_SRC_DIR@/generic \
#	-I@TCL_SRC_DIR@/generic \
#	@TCL_INCLUDES@ @TCLINT_INCLUDES@ \
#	@TK_INCLUDES@ @TKINT_INCLUDES@
#
#AM_CFLAGS = @X_CFLAGS@ @TCL_CFLAGS@  @TK_CFLAGS@ @TK_DEFS@ \
#	-UPACKAGE_NAME -UPACKAGE_STRING -UPACKAGE_TARNAME -UPACKAGE_VERSION
#
#if WITH_TK
#pkgtcl_LTLIBRARIES = libtkspline.la
#man_MANS = $(man)
#pdf_DATA = $(pdf)
#endif
#
#libtkspline_la_LDFLAGS = -no-undefined 
#libtkspline_la_SOURCES = tkspline.c
#libtkspline_la_LIBADD = \
#	$(top_builddir)/tclpkg/tkstubs/libtkstubs_C.la \
#	$(top_builddir)/tclpkg/tclstubs/libtclstubs_C.la @TK_LIB_SPEC@
#
#if WITH_TK
#all-local: pkgIndex.tcl
#
#pkgIndex.tcl: libtkspline.la
#	sh $(top_srcdir)/tclpkg/mkpkgindex.sh libtkspline.la Tkspline $(VERSION)
#endif
#
#.n.pdf:
#	groff -Tps -man $< | ps2pdf - - >$@
#
#EXTRA_DIST = $(man) $(pdf) dllEntry.c
#
#CLEANFILES = pkgIndex.tcl
#
#DISTCLEANFILES = $(pdf)
