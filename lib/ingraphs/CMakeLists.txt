# $Id$ $Revision$
## Process this file with cmake to produce Makefile

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/lib/graph
	${CMAKE_SOURCE_DIR}/lib/cdt
)


########### next target ###############

SET(ingraphs_SRCS
	ingraphs.c
)

ADD_LIBRARY(ingraphs STATIC ${ingraphs_SRCS})


########### install files ###############




#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#AM_CPPFLAGS = \
#        -I$(top_srcdir) \
#        -I$(top_srcdir)/lib/graph \
#	-I$(top_srcdir)/lib/cdt 
#
#noinst_HEADERS = ingraphs.h 
#noinst_LTLIBRARIES = libingraphs_C.la
#
#libingraphs_C_la_SOURCES = ingraphs.c
#
#EXTRA_DIST = Makefile.old
