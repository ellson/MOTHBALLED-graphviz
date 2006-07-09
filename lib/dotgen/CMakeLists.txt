# $Id$ $Revision$

INCLUDE(SRCDIRs)
INCLUDE(${dotgen_SRCDIR}/sources.cmake)

ADD_LIBRARY(dotgen STATIC ${dotgen_SRCS})
