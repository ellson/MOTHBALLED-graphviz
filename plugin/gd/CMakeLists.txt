# $Id$ $Revision$
## Process this file with cmake to produce Makefile

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/lib/common
	${CMAKE_SOURCE_DIR}/lib/cdt
	${CMAKE_SOURCE_DIR}/lib/graph
	${CMAKE_SOURCE_DIR}/lib/pathplan
	${CMAKE_SOURCE_DIR}/lib/gvc
	${CMAKE_SOURCE_DIR}/lib/gd
)

########### next target ###############

SET(gvplugin_gd_SRCS
    gvplugin_gd.c
    gvrender_gd.c
    gvtextlayout_gd.c
    gvloadimage_gd.c
)
ADD_LIBRARY(gvplugin_gd MODULE ${gvplugin_gd_SRCS})

ADD_LIBRARY(gvplugin_gd_static STATIC ${gvplugin_gd_SRCS})

########### install files ###############
