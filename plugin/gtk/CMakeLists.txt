# $Id$ $Revision$

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/directories.cmake)
INCLUDE(${gvplugin_gtk_SRCDIR}/sources.cmake)

ADD_LIBRARY(gvplugin_gtk MODULE ${gvplugin_gtk_SRCS})

ADD_LIBRARY(gvplugin_gtk_static STATIC ${gvplugin_gtk_SRCS})
