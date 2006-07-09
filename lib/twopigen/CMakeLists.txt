# $Id$ $Revision$

INCLUDE(SRCDIRs)
INCLUDE(${twopigen_SRCDIR}/sources.cmake)

ADD_LIBRARY(twopigen STATIC ${twopigen_SRCS})
