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
)

########### next target ###############

SET(gvplugin_dot_layout_SRCS
    gvplugin_dot_layout.c
    gvlayout_dot_layout.c
)
ADD_LIBRARY(gvplugin_dot_layout MODULE ${gvplugin_dot_layout_SRCS})

ADD_LIBRARY(gvplugin_dot_layout_static STATIC ${gvplugin_dot_layout_SRCS})

########### install files ###############
