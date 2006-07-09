# $Id$ $Revision$

INCLUDE(SRCDIRs)
INCLUDE(${fdpgen_SRCDIR}/sources.cmake)

ADD_LIBRARY(fdpgen STATIC ${fdpgen_SRCS})
