# $Id$ $Revision$
## Process this file with cmake to produce Makefile

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/lib/common
	${CMAKE_SOURCE_DIR}/lib/gvc
	${CMAKE_SOURCE_DIR}/lib/neatogen
	${CMAKE_SOURCE_DIR}/lib/pack
	${CMAKE_SOURCE_DIR}/lib/pathplan
	${CMAKE_SOURCE_DIR}/lib/graph
	${CMAKE_SOURCE_DIR}/lib/cdt
)


########### next target ###############

SET(twopigen_SRCS
	twopiinit.c
	circle.c
)

ADD_LIBRARY(twopigen STATIC ${twopigen_SRCS})


########### install files ###############




#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#AM_CPPFLAGS = \
#        -I$(top_srcdir) \
#        -I$(top_srcdir)/lib/common \
#	-I$(top_srcdir)/lib/gvc \
#	-I$(top_srcdir)/lib/neatogen \
#	-I$(top_srcdir)/lib/pack \
#	-I$(top_srcdir)/lib/pathplan \
#	-I$(top_srcdir)/lib/graph \
#	-I$(top_srcdir)/lib/cdt
#
#noinst_HEADERS = circle.h
#noinst_LTLIBRARIES = libtwopigen_C.la
#
#libtwopigen_C_la_SOURCES = twopiinit.c circle.c
#
#EXTRA_DIST = Makefile.old
