# $Id$ $Revision$
## Process this file with cmake to produce Makefile

INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/lib/agraph
	${CMAKE_SOURCE_DIR}/lib/cdt
)

########### next target ###############

INCLUDE(${CMAKE_SOURCE_DIR}/lib/agutil/sources.cmake)

ADD_LIBRARY(agutil STATIC ${agutil_SRCS})


########### install files ###############




#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#AM_CPPFLAGS = \
#        -I$(top_srcdir) \
#        -I$(top_srcdir)/lib/agraph \
#        -I$(top_srcdir)/lib/cdt
#
#noinst_HEADERS = agutil.h
#noinst_LTLIBRARIES = libagutil_C.la
#
#libagutil_C_la_SOURCES = dynattr.c nodeq.c agutil.h
