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

SET(circogen_SRCS
	circularinit.c
	nodelist.c
	block.c
	edgelist.c
	stack.c
	circular.c
	deglist.c
	blocktree.c
	blockpath.c
	circpos.c
	nodeset.c
)

ADD_LIBRARY(circogen STATIC ${circogen_SRCS})


########### install files ###############




#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#pkgconfigdir = $(libdir)/pkgconfig
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
#noinst_HEADERS = block.h blockpath.h blocktree.h circo.h \
#	circpos.h circular.h deglist.h edgelist.h nodelist.h \
#	nodeset.h stack.h
#noinst_LTLIBRARIES = libcircogen_C.la
#
#libcircogen_C_la_SOURCES = circularinit.c nodelist.c block.c edgelist.c \
#	stack.c circular.c deglist.c blocktree.c blockpath.c \
#	circpos.c nodeset.c
#
#EXTRA_DIST = Makefile.old
