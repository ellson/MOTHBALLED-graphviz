# $Id$ $Revision$

INCLUDE(SRCDIRs)
INCLUDE(${ingraphs_SRCDIR}/sources.cmake)

ADD_LIBRARY(ingraphs STATIC ${ingraphs_SRCS})
