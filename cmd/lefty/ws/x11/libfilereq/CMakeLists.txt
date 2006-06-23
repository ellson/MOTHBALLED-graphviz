# $Id$ $Revision$
## Process this file with cmake to produce Makefile

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_BINARY_DIR}
)


########### next target ###############

SET(filereq_SRCS
	Dir.c
	Draw.c
	Path.c
	SelFile.c
)

ADD_LIBRARY(filereq STATIC ${filereq_SRCS})


########### install files ###############




#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#AM_CPPFLAGS = @X_CFLAGS@ \
#	$(XAW_INCLUDES)
#
#noinst_HEADERS = SFinternal.h SFDecls.h xstat.h SF.h
#if WITH_X
#if WITH_XAW
#noinst_LTLIBRARIES = libfilereq.la
#endif
#endif
#
#libfilereq_la_SOURCES = Dir.c Draw.c Path.c SelFile.c
#libfilereq_la_LIBADD = $(XAW_LIBS)
#
#EXTRA_DIST = README.selfile
