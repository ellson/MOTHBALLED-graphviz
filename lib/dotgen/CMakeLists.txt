# $Id$ $Revision$

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/directories.cmake)
INCLUDE(${dotgen_SRCDIR}/sources.cmake)

ADD_LIBRARY(dotgen STATIC ${dotgen_SRCS})
