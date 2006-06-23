# $Id$ $Revision$
## Process this file with cmake to produce Makefile

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/sfio
)


########### next target ###############

SET(ast_SRCS
	pathpath.c
	sfstr.h
	chresc.c
	chrtoi.c
	error.c
	fmtbuf.c
	fmtesc.c
	pathaccess.c
	pathbin.c
	pathcanon.c
	pathcat.c
	pathexists.c
	pathfind.c
	pathgetlink.c
	strcopy.c
	stresc.c
	strmatch.c
	strton.c
	strerror.c
)

ADD_LIBRARY(ast STATIC ${ast_SRCS})


########### install files ###############




#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#AM_CPPFLAGS = -I$(top_srcdir)/lib/sfio
#
#noinst_HEADERS = align.h ast.h error.h hashkey.h
#noinst_LTLIBRARIES = libast_C.la
#
#libast_C_la_SOURCES = pathpath.c sfstr.h chresc.c chrtoi.c error.c \
#	fmtbuf.c fmtesc.c pathaccess.c pathbin.c pathcanon.c pathcat.c \
#	pathexists.c pathfind.c pathgetlink.c \
#	strcopy.c stresc.c strmatch.c strton.c strerror.c
#
#EXTRA_DIST = Makefile.old
