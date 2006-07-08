# $Id$ $Revision$

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/directories.cmake)
INCLUDE(${neatogen_SRCDIR}/sources.cmake)

ADD_LIBRARY(neatogen STATIC ${neatogen_SRCS})
