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

SET(gvplugin_core_SRCS
    gvplugin_core.c
    gvrender_core_svg.c
    gvrender_core_ps.c
    gvrender_core_map.c
    gvloadimage_core.c
)
ADD_LIBRARY(gvplugin_core MODULE ${gvplugin_core_SRCS})

ADD_LIBRARY(gvplugin_core_static STATIC ${gvplugin_core_SRCS})

########### install files ###############
