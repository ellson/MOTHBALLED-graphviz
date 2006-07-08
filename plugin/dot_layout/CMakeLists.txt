# $Id$ $Revision$

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/directories.cmake)
INCLUDE(${gvplugin_dot_layout_SRCDIR}/sources.cmake)

ADD_LIBRARY(gvplugin_dot_layout MODULE ${gvplugin_dot_layout_SRCS})

ADD_LIBRARY(gvplugin_dot_layout_static STATIC ${gvplugin_dot_layout_SRCS})
