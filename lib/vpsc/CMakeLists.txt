# $Id$ $Revision$

INCLUDE(SRCDIRs)
INCLUDE(${vpsc_SRCDIR}/sources.cmake)

ADD_LIBRARY(vpsc STATIC ${vpsc_SRCS})
