# $Id$ $Revision$

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/directories.cmake)
INCLUDE(${circogen_SRCDIR}/sources.cmake)

ADD_LIBRARY(circogen STATIC ${circogen_SRCS})
