# $Id$ $Revision$
## Process this file with cmake to produce Makefile

ADD_SUBDIRECTORY( libfilereq ) 

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_BINARY_DIR}
	${CMAKE_SOURCE_DIR}/cmd/lefty
	${CMAKE_SOURCE_DIR}/cmd/lefty/ws/x11/libfilereq
)


########### next target ###############

SET(ws_SRCS
	garray.c
	gbutton.c
	gcanvas.c
	gcommon.c
	glabel.c
	gmenu.c
	gpcanvas.c
	gquery.c
	gscroll.c
	gtext.c
	gview.c
)

ADD_LIBRARY(ws STATIC ${ws_SRCS})


########### install files ###############




#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#SUBDIRS = libfilereq
#
#AM_CPPFLAGS = -I$(top_srcdir)/cmd/lefty \
#	-I$(top_srcdir)/cmd/lefty/ws/x11/libfilereq \
#	-DFEATURE_X11 @XAW_INCLUDES@ @X_CFLAGS@
#
#noinst_HEADERS = gcommon.h
#if WITH_X
#if WITH_XAW
#noinst_LTLIBRARIES = libws.la
#endif
#endif
#
#libws_la_SOURCES = garray.c gbutton.c gcanvas.c gcommon.c glabel.c \
#	gmenu.c gpcanvas.c gquery.c gscroll.c gtext.c gview.c
