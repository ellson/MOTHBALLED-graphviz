# $Id$ $Revision$
## Process this file with cmake to produce Makefile

INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}
)

ADD_CUSTOM_COMMAND(
	OUTPUT cdt.pdf
	DEPENDS cdt.3
	COMMAND groff -Tps -man cdt.3 | ps2pdf - cdt.pdf
)

########### next target ###############

INCLUDE(${CMAKE_SOURCE_DIR}/lib/cdt/sources.cmake)

ADD_LIBRARY(cdt STATIC ${cdt_SRCS})

########### install files ###############

INSTALL_FILES(FILES
	libcdt.pc
)
INSTALL_FILES(FILES
	cdt.pdf
)
INSTALL_FILES(FILES
	cdt.1
)


#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#pdfdir = $(pkgdatadir)/doc/pdf
#pkgextraincludedir = $(pkgincludedir)
#pkgconfigdir = $(libdir)/pkgconfig
#
#man = cdt.3
#pdf = cdt.pdf
#
#AM_CPPFLAGS = -I$(top_srcdir)
#
#pkginclude_HEADERS = cdt.h dthdr.h
#noinst_LTLIBRARIES = libcdt_C.la
#pkglib_LTLIBRARIES = libcdt.la
#man_MANS = $(man)
#pdf_DATA = $(pdf)
#pkgextrainclude_DATA = $(top_builddir)/ast_common.h
#pkgconfig_DATA = libcdt.pc
#
#libcdt_C_la_SOURCES = dtclose.c dtdisc.c dtextract.c dtflatten.c \
#	dthash.c dtlist.c dtmethod.c dtopen.c dtrenew.c dtrestore.c dtsize.c \
#	dtstat.c dtstrhash.c dttree.c dtview.c dtwalk.c
#
#libcdt_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined
#libcdt_la_SOURCES = $(libcdt_C_la_SOURCES)
#
#.3.pdf:
#	groff -Tps -man $< | ps2pdf - - >$@
#
#EXTRA_DIST = $(man) $(pdf) Makefile.old features
#
#DISTCLEANFILES = $(pdf)
