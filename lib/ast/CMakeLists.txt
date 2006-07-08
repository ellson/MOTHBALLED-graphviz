# $Id$ $Revision$

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/directories.cmake)
INCLUDE(${ast_SRCDIR}/sources.cmake)

ADD_LIBRARY(ast STATIC ${ast_SRCS})
