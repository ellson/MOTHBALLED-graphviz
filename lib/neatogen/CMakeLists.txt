# $Id$ $Revision$

INCLUDE(SRCDIRs)
INCLUDE(${neatogen_SRCDIR}/sources.cmake)

ADD_LIBRARY(neatogen STATIC ${neatogen_SRCS})
