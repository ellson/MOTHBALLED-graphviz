# $Id$ $Revision$

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/directories.cmake)
INCLUDE(${vmalloc_SRCDIR}/sources.cmake)

ADD_LIBRARY(vmalloc STATIC ${vmalloc_SRCS})
