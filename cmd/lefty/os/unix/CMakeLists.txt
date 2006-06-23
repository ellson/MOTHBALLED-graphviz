# $Id$ $Revision$
## Process this file with cmake to produce Makefile

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_BINARY_DIR}
	${CMAKE_SOURCE_DIR}/cmd/lefty
)


########### next target ###############

SET(os_SRCS
	io.c
)

ADD_LIBRARY(os STATIC ${os_SRCS})


########### install files ###############




#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#AM_CPPFLAGS = -I$(top_srcdir)/cmd/lefty
#
#if WITH_X
#noinst_LTLIBRARIES = libos.la
#endif
#
#libos_la_SOURCES = io.c
#
#EXTRA_DIST = $(libos_la_SOURCES)
