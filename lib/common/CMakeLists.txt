# $Id$ $Revision$

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/directories.cmake)
INCLUDE(${common_SRCDIR}/sources.cmake)

ADD_LIBRARY(common STATIC ${common_SRCS})
