# $Id$ $Revision$

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/directories.cmake)
INCLUDE(${ingraphs_SRCDIR}/sources.cmake)

ADD_LIBRARY(ingraphs STATIC ${ingraphs_SRCS})
