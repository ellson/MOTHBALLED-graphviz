# $Id$ $Revision$

INCLUDE(SRCDIRs)
INCLUDE(${ast_SRCDIR}/sources.cmake)

ADD_LIBRARY(ast STATIC ${ast_SRCS})
