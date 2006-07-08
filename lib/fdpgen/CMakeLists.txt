# $Id$ $Revision$

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/directories.cmake)
INCLUDE(${fdpgen_SRCDIR}/sources.cmake)

ADD_LIBRARY(fdpgen STATIC ${fdpgen_SRCS})
