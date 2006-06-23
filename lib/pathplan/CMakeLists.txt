# $Id$ $Revision$
## Process this file with cmake to produce Makefile

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}
)


########### next target ###############

SET(pathplan_SRCS
	cvt.c
	inpoly.c
	route.c
	shortest.c
	shortestpth.c
	solvers.c
	triang.c
	util.c
	visibility.c
)

ADD_LIBRARY(pathplan STATIC ${pathplan_SRCS})


########### install files ###############

INSTALL_FILES(FILES
	pathplan.pdf
)
INSTALL_FILES(FILES
	pathplan.3
)
INSTALL_FILES(FILES
	libpathplan.pc
)



#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#pdfdir = $(pkgdatadir)/doc/pdf
#pkgconfigdir = $(libdir)/pkgconfig
#
#man = pathplan.3
#pdf = pathplan.pdf
#
#pkginclude_HEADERS = pathgeom.h pathplan.h pathutil.h solvers.h \
#	tri.h vis.h vispath.h
#noinst_LTLIBRARIES = libpathplan_C.la
#pkglib_LTLIBRARIES = libpathplan.la
#pkgconfig_DATA = libpathplan.pc
#man_MANS = $(man)
#pdf_DATA = $(pdf)
#
#libpathplan_C_la_SOURCES = cvt.c inpoly.c route.c shortest.c \
#	shortestpth.c solvers.c triang.c util.c visibility.c
#
#libpathplan_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined
#libpathplan_la_SOURCES = $(libpathplan_C_la_SOURCES)
#libpathplan_la_LIBADD = @MATH_LIBS@
#
#.3.pdf:
#	groff -Tps -man $< | ps2pdf - - >$@
#
#EXTRA_DIST = $(man) $(pdf) Makefile.old path.lefty
#
#DISTCLEANFILES = $(pdf)
