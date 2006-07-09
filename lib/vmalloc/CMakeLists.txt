# $Id$ $Revision$

INCLUDE(SRCDIRs)
INCLUDE(${vmalloc_SRCDIR}/sources.cmake)

ADD_LIBRARY(vmalloc STATIC ${vmalloc_SRCS})
