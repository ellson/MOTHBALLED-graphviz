# $Id$ $Revision$

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/directories.cmake)
INCLUDE(${twopigen_SRCDIR}/sources.cmake)

ADD_LIBRARY(twopigen STATIC ${twopigen_SRCS})
