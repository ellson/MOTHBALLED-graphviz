# $Id$ $Revision$

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/directories.cmake)
INCLUDE(${agutil_SRCDIR}/sources.cmake)

ADD_LIBRARY(agutil STATIC ${agutil_SRCS})
