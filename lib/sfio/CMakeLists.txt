# $Id$ $Revision$

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/directories.cmake)
INCLUDE(${sfio_SRCDIR}/sources.cmake)

ADD_LIBRARY(sfio STATIC ${sfio_SRCS})
