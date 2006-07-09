# $Id$ $Revision$

INCLUDE(SRCDIRs)
INCLUDE(${circogen_SRCDIR}/sources.cmake)

ADD_LIBRARY(circogen STATIC ${circogen_SRCS})
