# $Id$ $Revision$
## Process this file with cmake to produce Makefile

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_BINARY_DIR}
)


########### next target ###############

SET(tclhandle_SRCS
	tclhandle.c
)

ADD_LIBRARY(tclhandle STATIC ${tclhandle_SRCS})


########### install files ###############




#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#noinst_HEADERS = tclhandle.h
#if WITH_TCL
#noinst_LTLIBRARIES = libtclhandle_C.la
#endif
#
#libtclhandle_C_la_SOURCES = tclhandle.c
#
#EXTRA_DIST = Makefile.old
