# $Id$ $Revision$
## Process this file with cmake to produce Makefile

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/lib/common
	${CMAKE_SOURCE_DIR}/lib/gvc
	${CMAKE_SOURCE_DIR}/lib/neatogen
	${CMAKE_SOURCE_DIR}/lib/pathplan
	${CMAKE_SOURCE_DIR}/lib/graph
	${CMAKE_SOURCE_DIR}/lib/cdt
)


########### next target ###############

SET(pack_SRCS
	ccomps.c
	pack.c
)

ADD_LIBRARY(pack STATIC ${pack_SRCS})


########### install files ###############

INSTALL_FILES(FILES
	pack.pdf
)
INSTALL_FILES(FILES
	pack.3
)



#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#pdfdir = $(pkgdatadir)/doc/pdf
#
#AM_CPPFLAGS = -I$(top_srcdir) \
#        -I$(top_srcdir)/lib/common \
#	-I$(top_srcdir)/lib/gvc \
#	-I$(top_srcdir)/lib/neatogen \
#	-I$(top_srcdir)/lib/pathplan \
#	-I$(top_srcdir)/lib/graph \
#	-I$(top_srcdir)/lib/cdt
#
#pkginclude_HEADERS = pack.h
#noinst_LTLIBRARIES = libpack_C.la
#pkglib_LTLIBRARIES = libpack.la
#man_MANS = pack.3
#pdf_DATA = pack.pdf
#
#libpack_C_la_SOURCES = ccomps.c pack.c
#
#libpack_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined
#libpack_la_SOURCES = $(libpack_C_la_SOURCES)
#libpack_la_LIBADD = \
#	$(top_builddir)/lib/graph/libgraph.la \
#	$(top_builddir)/lib/gvc/libgvc.la
#
#.3.pdf:
#	groff -Tps -man $< | ps2pdf - - >$@
#
#EXTRA_DIST = $(man_MANS) $(pdf_DATA) Makefile.old ptest.c
#
#DISTCLEANFILES = $(pdf_DATA)
