# $Id$ $Revision$
## Process this file with cmake to produce Makefile

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/lib/common
	${CMAKE_SOURCE_DIR}/lib/gvc
	${CMAKE_SOURCE_DIR}/lib/graph
	${CMAKE_SOURCE_DIR}/lib/cdt
	${CMAKE_SOURCE_DIR}/lib/pathplan
)


########### next target ###############

SET(dotgen_SRCS
	acyclic.c
	class1.c
	class2.c
	cluster.c
	compound.c
	conc.c
	decomp.c
	fastgr.c
	flat.c
	dotinit.c
	mincross.c
	position.c
	rank.c
	sameport.c
	dotsplines.c
)

ADD_LIBRARY(dotgen STATIC ${dotgen_SRCS})


########### install files ###############




#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#AM_CPPFLAGS = \
#	-I$(top_srcdir) \
#        -I$(top_srcdir)/lib/common \
#        -I$(top_srcdir)/lib/gvc \
#	-I$(top_srcdir)/lib/graph \
#	-I$(top_srcdir)/lib/cdt \
#	-I$(top_srcdir)/lib/pathplan
#
#noinst_HEADERS = dot.h dotprocs.h
#noinst_LTLIBRARIES = libdotgen_C.la
#
#libdotgen_C_la_LDFLAGS = -no-undefined
#libdotgen_C_la_SOURCES = acyclic.c class1.c class2.c cluster.c compound.c \
#	conc.c decomp.c fastgr.c flat.c dotinit.c mincross.c \
#	position.c rank.c sameport.c dotsplines.c
#
#EXTRA_DIST = Makefile.old
