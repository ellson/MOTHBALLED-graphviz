# $Id$ $Revision$

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/directories.cmake)
INCLUDE(${gvplugin_core_SRCDIR}/sources.cmake)

ADD_LIBRARY(gvplugin_core MODULE ${gvplugin_core_SRCS})

ADD_LIBRARY(gvplugin_core_static STATIC ${gvplugin_core_SRCS})
