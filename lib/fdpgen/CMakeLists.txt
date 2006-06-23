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

SET(fdpgen_SRCS
	comp.c
	dbg.c
	grid.c
	fdpinit.c
	layout.c
	tlayout.c
	xlayout.c
	clusteredges.c
)

ADD_LIBRARY(fdpgen STATIC ${fdpgen_SRCS})


########### install files ###############




#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#AM_CPPFLAGS = \
#        -I$(top_srcdir) \
#        -I$(top_srcdir)/lib/common \
#        -I$(top_srcdir)/lib/gvc \
#        -I$(top_srcdir)/lib/neatogen \
#        -I$(top_srcdir)/lib/pack \
#        -I$(top_srcdir)/lib/pathplan \
#        -I$(top_srcdir)/lib/graph \
#        -I$(top_srcdir)/lib/cdt
#
#noinst_HEADERS = comp.h dbg.h fdp.h grid.h tlayout.h xlayout.h \
#	clusteredges.h
#noinst_LTLIBRARIES = libfdpgen_C.la
#
#libfdpgen_C_la_SOURCES = comp.c dbg.c grid.c fdpinit.c layout.c \
#	tlayout.c xlayout.c clusteredges.c
#
#EXTRA_DIST = Makefile.old
