# $Id$ $Revision$
## Process this file with cmake to produce Makefile

INCLUDE( UsePkgConfig )

PKGCONFIG( pangocairo PANGOCAIRO_INCLUDE_DIR PANGOCAIRO_LIB_DIR PANGOCAIRO_LINK_FLAGS PANGOCAIRO_CFLAGS )

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/lib/common
	${CMAKE_SOURCE_DIR}/lib/cdt
	${CMAKE_SOURCE_DIR}/lib/graph
	${CMAKE_SOURCE_DIR}/lib/pathplan
	${CMAKE_SOURCE_DIR}/lib/gvc
	${PANGOCAIRO_INCLUDE_DIR}
)

########### next target ###############

ADD_DEFINITIONS(${PANGOCAIRO_CFLAGS})
SET(gvplugin_pango_SRCS
    gvplugin_pango.c
    gvrender_pango.c
    gvtextlayout_pango.c
    gvloadimage_pango.c
)
ADD_LIBRARY(gvplugin_pango MODULE ${gvplugin_pango_SRCS})

ADD_LIBRARY(gvplugin_pango_static STATIC ${gvplugin_pango_SRCS})

########### install files ###############
