# $Id$ $Revision$

INCLUDE(SRCDIRs)
INCLUDE(${tclhandle_SRCDIR}/sources.cmake)

ADD_LIBRARY(tclhandle STATIC ${tclhandle_SRCS})
