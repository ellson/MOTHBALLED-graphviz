# $Id$ $Revision$

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/directories.cmake)
INCLUDE(${vpsc_SRCDIR}/sources.cmake)

ADD_LIBRARY(vpsc STATIC ${vpsc_SRCS})
