# $Id$ $Revision$

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/directories.cmake)
INCLUDE(${gvplugin_gd_SRCDIR}/sources.cmake)

ADD_LIBRARY(gvplugin_gd MODULE ${gvplugin_gd_SRCS})

ADD_LIBRARY(gvplugin_gd_static STATIC ${gvplugin_gd_SRCS})
